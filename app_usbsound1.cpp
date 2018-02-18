/*
buzzer: D5
*/

#include <avr/pgmspace.h>
#include "busby.h"
#include "usbsound.h"

#ifndef F_CPU
#define F_CPU 16000000
#endif

static const DescDev PROGMEM devDesc =
{
    sizeof(DescDev),
    DTYPE_DEVICE,
    0x0110,
    0,
    0,
    0,
    8,
    0x03eb,
    0x2046,
    0x0002,
    STRING_ID_MANUFACTURER,
    STRING_ID_PRODUCT,
    0,
    1
};

struct MyConf
{
    DescConf config;
    DescIface audioCtrlIface;
    DescAudioIface audioIface;
    DescAudioInTerminal inTerm;
    DescAudioOutTerminal outTerm;
    DescIface stream0;
    DescIface stream1;
    DescAudioIfaceAS ifaceSpc;
    AudioFormat format;
    SampleFreq freqs[5];
    AudioEndpoint stream;
    AudioEndpointSpc endpointSpc;
};

static const MyConf PROGMEM myConf
{
    {
        sizeof(DescConf),
        DTYPE_CONFIGURATION,
        sizeof(MyConf),
        2, // 2 interfaces
        1, // configuration 1
        0, // no descriptor
        USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED,
        USB_CONFIG_POWER_MA(100)
    },
    {
        sizeof(DescIface),
        DTYPE_INTERFACE,
        INTERFACE_ID_AudioControl,
        0,
        0,
        AUDIO_CSCP_AUDIOCLASS,
        AUDIO_CSCP_CONTROLSUBCLASS,
        AUDIO_CSCP_CONTROLPROTOCOL,
        0
    },
    {
        sizeof(DescAudioIface),
        DTYPE_CSINTERFACE,
        AUDIO_DSUBTYPE_CSINTERFACE_HEADER,
        0x0100,
        sizeof(DescAudioIface) + sizeof(DescAudioInTerminal) + sizeof(DescAudioOutTerminal),
        1,
        INTERFACE_ID_AudioStream
    },
    {
        sizeof(DescAudioInTerminal),
        DTYPE_CSINTERFACE,
        AUDIO_DSUBTYPE_CSINTERFACE_INPUTTERMINAL,
        1,  // terminal ID = 1
        AUDIO_TERMINAL_STREAMING,
        0,
        2,  // 2 channels
        AUDIO_CHANNEL_LEFT_FRONT | AUDIO_CHANNEL_RIGHT_FRONT,
        0,
        0
    },
    {
        sizeof(DescAudioOutTerminal),
        DTYPE_CSINTERFACE,
        AUDIO_DSUBTYPE_CSINTERFACE_OUTPUTTERMINAL,
        2, // terminal ID = 2
        AUDIO_TERMINAL_OUT_SPEAKER,
        0,
        1,
        0
    },
    {
        sizeof(DescIface),
        DTYPE_INTERFACE,
        INTERFACE_ID_AudioStream,
        0,
        0,
        AUDIO_CSCP_AUDIOCLASS,
        AUDIO_CSCP_AUDIOSTREAMINGSUBCLASS,
        AUDIO_CSCP_STREAMINGPROTOCOL,
        0
    },
    {
        sizeof(DescIface),
        DTYPE_INTERFACE,
        INTERFACE_ID_AudioStream,
        1,
        1,
        AUDIO_CSCP_AUDIOCLASS,
        AUDIO_CSCP_AUDIOSTREAMINGSUBCLASS,
        AUDIO_CSCP_STREAMINGPROTOCOL,
        0
    },
    {
        sizeof(DescAudioIfaceAS),
        DTYPE_CSINTERFACE,
        AUDIO_DSUBTYPE_CSInterface_General,
        1,
        1,
        0x0001
    },
    {
        sizeof(AudioFormat) + sizeof(myConf.freqs),
        DTYPE_CSINTERFACE,
        AUDIO_DSUBTYPE_CSInterface_FormatType,
        0x01,
        0x02,
        0x02,
        16,
        sizeof(myConf.freqs) / sizeof(SampleFreq)
    },
    {
        {0x40, 0x1f, 0x00}, // 8000
        {0x11, 0x2b, 0x00}, // 11025
        {0x22, 0x56, 0x00}, // 22050
        {0x44, 0xac, 0x00}, // 44100
        {0x80, 0xbb, 0x00}  // 48000
    },
    {
        {
            sizeof(AudioEndpoint),
            DTYPE_ENDPOINT,
            AUDIO_STREAM_EPADDR,
            EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_SYNC | ENDPOINT_USAGE_DATA,
            AUDIO_STREAM_EPSIZE,
            0x01
        },
        0,
        0
    },
    {
        sizeof(AudioEndpointSpc),
        DTYPE_CSENDPOINT,
        AUDIO_DSUBTYPE_CSEndpoint_General,
        AUDIO_EP_ACCEPTS_SMALL_PACKETS | AUDIO_EP_SAMPLE_FREQ_CONTROL,
        0,
        0
    }
};

static const DescString<2> PROGMEM languageString =
{
    USB_STRING_LEN(1),
    DTYPE_STRING,
    (wchar_t)0x0409
};

static const DescString<12> PROGMEM manufacturerString =
{
    USB_STRING_LEN(11),
    DTYPE_STRING,
    L"Dean Camera"
};

static const DescString<11> PROGMEM productString =
{
    USB_STRING_LEN(10),
    DTYPE_STRING,
    L"Audio Demo"
};

class USBSound : public USB
{
private:
    Endpoint _inpoint;
    uint32_t currentFreq = 48000;
public:
    void customCtrl();
    void connect();
    void configure();
    void sampleCallback();
    uint16_t getDesc(uint16_t wValue, uint16_t wIndex, const void **const descAddr);
    USBSound();
};

uint16_t USBSound::getDesc(uint16_t wValue, uint16_t wIndex, const void **const descAddr)
{
    const void *addr = NULL;
    uint16_t size = 0;

    switch (wValue >> 8)
    {
    case DTYPE_DEVICE:
        addr = &devDesc;
        size = sizeof(DescDev);
        break;
    case DTYPE_CONFIGURATION:
        addr = &myConf;
        size = sizeof(MyConf);
        break;
    case DTYPE_STRING:
        switch (wValue & 0xff)
        {
        case STRING_ID_LANGUAGE:
            addr = &languageString;
            size = pgm_read_byte(&languageString.size);
            break;
        case STRING_ID_MANUFACTURER:
            addr = &manufacturerString;
            size = pgm_read_byte(&manufacturerString.size);
            break;
        case STRING_ID_PRODUCT:
            addr = &productString;
            size = pgm_read_byte(&productString.size);
            break;
        }
        break;
    }

    *descAddr = addr;
    return size;
}

static USBSound *g_usbSound;

static bool StreamingAudioInterfaceSelected = false;

void USBSound::customCtrl()
{
    const uint8_t bmRequestType = _ctrlReq.bmRequestType;

    switch (_ctrlReq.bRequest)
    {
    case REQ_SETINTERFACE:
        if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_INTERFACE))
        {
            *p_ueintx &= ~(1<<rxstpi);
            clearStatusStage();
            StreamingAudioInterfaceSelected = _ctrlReq.wValue != 0;
        }
        break;
    case AUDIO_REQ_SETCURRENT:
        if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_ENDPOINT))
        {
            uint8_t epAddr = (uint8_t)_ctrlReq.wIndex;
            uint8_t epCtrl = _ctrlReq.wValue >> 8;

            if (epAddr == AUDIO_STREAM_EPADDR && epCtrl == AUDIO_EPCONTROL_SamplingFreq)
            {
                uint8_t sampleRate[3];
                *p_ueintx &= ~(1<<rxstpi);
                readControlStreamLE(sampleRate, sizeof(sampleRate));
                *p_ueintx &= ~(1<<txini | 1<<fifocon);  // clear in

                currentFreq = (uint32_t)sampleRate[2] << 16 | (uint32_t)sampleRate[1] << 8 |
                    (uint32_t)sampleRate[0];

                *p_ocr0a = (F_CPU >> 3) / currentFreq - 1;
            }
        }
        break;
    case AUDIO_REQ_GETSTATUS:
        if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE) ||
            bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_ENDPOINT))
        {
            *p_ueintx &= ~(1<<rxstpi);
            clearStatusStage();
        }
        break;
    case AUDIO_REQ_GETCURRENT:
        if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_ENDPOINT))
        {
            uint8_t epAddr = (uint8_t)_ctrlReq.wIndex;
            uint8_t epCtrl = _ctrlReq.wValue >> 8;

            if (epAddr == AUDIO_STREAM_EPADDR && epCtrl == AUDIO_EPCONTROL_SamplingFreq)
            {
                uint8_t sampleRate[3];
                sampleRate[2] = currentFreq >> 16;
                sampleRate[1] = currentFreq >> 8;
                sampleRate[0] = currentFreq & 0xff;
                *p_ueintx &= ~(1<<rxstpi);  // clear setup
                write_Control_Stream_LE(sampleRate, sizeof(sampleRate));
                *p_ueintx &= ~(1<<rxouti | 1<<fifocon); // clear out
            }
        }
        break;
    }
}

void USBSound::configure()
{
    configureEndpoint(_inpoint);
    *p_udien |= 1<<sofe;
}

USBSound::USBSound() : _inpoint(AUDIO_STREAM_EPADDR, 256, EP_TYPE_ISOCHRONOUS, 2)
{
    *p_usbcon &= ~(1<<otgpade);
    *p_uhwcon |= 1<<uvrege;
    *p_pllfrq |= 1<<pdiv2;
    *p_usbcon &= ~(1<<vbuste);
    *p_udien = 0;
    *p_usbint = 0;
    *p_udint = 0;
    *p_usbcon &= ~(1<<usbe);
    *p_usbcon |= 1<<usbe;
    *p_usbcon &= ~(1<<frzclk);
    *p_pllcsr = 0;
    state = DEVICE_STATE_UNATTACHED;
    confNum = 0;
    _remoteWakeupEn = false;
    USB_Device_CurrentlySelfPowered = false;
    *p_udcon &= ~(1<<lsm);
    *p_usbcon |= 1<<vbuste;
    configureEndpoint(0, 0, 8, 1);
    *p_udint &= ~(1<<suspi);
    *p_udien |= 1<<suspe;
    *p_udien |= 1<<eorste;
    *p_udcon &= ~(1<<detach);
    *p_usbcon |= 1<<otgpade;
}

void USBSound::connect()
{
    *p_ddrc |= 1<<7;
    *p_ddrd |= 1<<5;
    *p_portc |= 1<<7;
    *p_portd |= 1<<5;
    *p_timsk0 = 1<<ocie0a;
    *p_ocr0a = (F_CPU >> 3) / currentFreq - 1;
    *p_tccr0a = 1<<wgm01;
    *p_tccr0b = 1<<cs01;
    *p_ddrc |= 1<<6 | 1<<5;
    *p_tccr3a = 1<<wgm30 | 1<<com3a1 | 1<<com3a0 | 1<<com3b1 | 1<<com3b0;
    *p_tccr3b = 1<<wgm32 | 1<<cs30;
}

//ISR(TIMER0_COMPA_vect)
extern "C" void __vector_21() __attribute__ ((signal, used, externally_visible));
void __vector_21()
{
    g_usbSound->sampleCallback();
}

void USBSound::sampleCallback()
{
    uint8_t prevEp = getCurrentEndpoint();
    _inpoint.select();

    if ((*p_ueintx & 1<<rxouti) && StreamingAudioInterfaceSelected)  // out received?
    {
        int8_t left = (int16_t)read16le() >> 8;
        int8_t right = (int16_t)read16le() >> 8;
        
        if ((*p_ueintx & 1<<rwal) == 0) // r/w not allowed?
            *p_ueintx &= ~(1<<rxouti | 1<<fifocon); // clear out

        *p_ocr3a = left ^ 1<<7;
        *p_ocr3b = right ^ 1<<7;
    }

    selectEndpoint(prevEp);
}

int main()
{
    USBSound usbSound;
    g_usbSound = &usbSound;
    zei();

    while (true)
        ;

    return 0;
}


