//TODO: verhaal hier wat dit doet

#include "usbmidi.h"
#include "usbsound.h"
#include <avr/pgmspace.h>

#define NULL 0

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
    0x2048,
    0x0001,
    STRING_ID_MANUFACTURER,
    STRING_ID_PRODUCT,
    0,
    1
};

struct MyConf
{
    DescConf config;
    DescIface controlIface;
    DescAudioIface controlSPC;
    DescIface streamIface;
    MIDIInterfaceAS midiAS;
    MIDIDescInputJack inJackEmb;
    MIDIDescInputJack inJackExt;
    MIDIDescOutputJack outJackEmb;
    MIDIDescOutputJack outJackExt;
    AudioEndpoint inJack;
    MIDIEndpoint inJackSPC;
    AudioEndpoint outJack;
    MIDIEndpoint outJackSPC;
};

static const DescDev PROGMEM DeviceDescriptor =
{
    sizeof(DescDev),
    DTYPE_DEVICE,
    0x0110,
    0,
    0,
    0,
    8,
    0x03EB,
    0x2048,
    0x0001,
    STRING_ID_MANUFACTURER,
    STRING_ID_PRODUCT,
    USE_INTERNAL_SERIAL,
    FIXED_NUM_CONFIGURATIONS
};

static const MyConf PROGMEM myConf =
{
    {
        sizeof(DescConf),
        DTYPE_CONFIGURATION,
        sizeof(MyConf),
        2,
        1,  // configuration number
        NO_DESCRIPTOR,
        USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED,
        USB_CONFIG_POWER_MA(100)
    },
    {
        sizeof(DescIface),
        DTYPE_INTERFACE,
        INTERFACE_ID_AUDIOCONTROL,
        0,
        0,  // total endpoints
        AUDIO_CSCP_AUDIOCLASS,
        AUDIO_CSCP_CONTROLSUBCLASS,
        AUDIO_CSCP_CONTROLPROTOCOL,
        NO_DESCRIPTOR
    },
    {
        sizeof(DescAudioIface),
        DTYPE_CSINTERFACE,
        AUDIO_DSUBTYPE_CSINTERFACE_HEADER,
        0x0100,
        sizeof(DescAudioIface),
        1,  // in collection
        INTERFACE_ID_AUDIOSTREAM
    },
    {
        sizeof(DescIface),
        DTYPE_INTERFACE,
        INTERFACE_ID_AUDIOSTREAM,
        0,
        2,  // 2 endpoints
        AUDIO_CSCP_AUDIOCLASS,
        AUDIO_CSCP_MIDISTREAMINGSUBCLASS,
        AUDIO_CSCP_STREAMINGPROTOCOL,
        NO_DESCRIPTOR
    },
    {
        sizeof(MIDIInterfaceAS),
        DTYPE_CSINTERFACE,
        AUDIO_DSUBTYPE_CSINTERFACE_HEADER,
        0x0100,
        sizeof(DescConf) // offsetof
    },
    {
        sizeof(MIDIDescInputJack),
        DTYPE_CSINTERFACE,
        AUDIO_DSUBTYPE_CSINTERFACE_INPUTTERMINAL,
        MIDI_JACKTYPE_EMBEDDED,
        1,  // jack id
        NO_DESCRIPTOR
    },
    {
        sizeof(MIDIDescInputJack),
        DTYPE_CSINTERFACE,
        AUDIO_DSUBTYPE_CSINTERFACE_INPUTTERMINAL,
        MIDI_JACKTYPE_EXTERNAL,
        2,  // jack id
        NO_DESCRIPTOR
    },
    {
        sizeof(MIDIDescOutputJack),
        DTYPE_CSINTERFACE,
        AUDIO_DSUBTYPE_CSINTERFACE_OUTPUTTERMINAL,
        MIDI_JACKTYPE_EMBEDDED,
        3,  // jack id
        1,  // 1 pin
        {0x02},
        {0x01},
        NO_DESCRIPTOR
    },
    {
        sizeof(MIDIDescOutputJack),
        DTYPE_CSINTERFACE,
        AUDIO_DSUBTYPE_CSINTERFACE_OUTPUTTERMINAL,
        MIDI_JACKTYPE_EXTERNAL,
        4,
        1,
        {0x01},
        {0x01},
        NO_DESCRIPTOR
    },
    {
        {
            sizeof(AudioEndpoint),
            DTYPE_ENDPOINT,
            MIDI_STREAM_OUT_EPADDR,
            EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
            MIDI_STREAM_EPSIZE,
            5
        },
        0,
        0
    },
    {
        sizeof(MIDIEndpoint),
        DTYPE_CSENDPOINT,
        AUDIO_DSUBTYPE_CSENDPOINT_GENERAL,
        1,
        {1}
    },
    {
        {
            sizeof(AudioEndpoint),
            DTYPE_ENDPOINT,
            MIDI_STREAM_IN_EPADDR,
            EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
            MIDI_STREAM_EPSIZE,
            5
        },
        0,
        0
    },
    {
        sizeof(MIDIEndpoint),
        DTYPE_CSENDPOINT,
        AUDIO_DSUBTYPE_CSENDPOINT_GENERAL,
        1,
        {3}
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

static const DescString<15> PROGMEM productString =
{
    USB_STRING_LEN(14),
    DTYPE_STRING,
    L"LUFA MIDI Demo"
};


class MIDI : public USB
{
private:
    Endpoint _inpoint;
    Endpoint _outpoint;
public:
    uint16_t getDesc(uint16_t wValue, uint16_t wIndex, const void ** const descAddr);
    void configure();
    void task();
    MIDI();
};

void MIDI::configure()
{
    configureEndpoint(_inpoint);
    configureEndpoint(_outpoint);
}

uint16_t MIDI::getDesc(uint16_t wValue, uint16_t wIndex, const void ** const descAddr)
{
    const void *addr = NULL;
    uint16_t size = 0;

    switch (wValue >> 8)
    {
    case DTYPE_DEVICE:
        addr = &DeviceDescriptor;
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

static constexpr uint8_t MIDI_EVENT(uint8_t cable, uint8_t command)
{
    return cable << 4 | command >> 4;
}

void MIDI::task()
{
    _inpoint.select();

    uint8_t midiCommand = 0, midiPitch;

    if ((*p_pinf & 1<<7) == 0)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3c;
    }

    if ((*p_pinf & 1<<6) == 0)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3d;
    }

    if ((*p_pinf & 1<<5) == 0)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3e;
    }

    if ((*p_pinf & 1<<4) == 0)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3f;
    }

    if ((*p_pinf & 1<<1) == 0)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x1f;
    }

    if ((*p_pinf & 1<<0) == 0)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x1e;
    }

    if (midiCommand)
    {
        MIDIEventPacket ev;
        ev.event = MIDI_EVENT(0, midiCommand);
        ev.data1 = midiCommand | 1; // channel
        ev.data2 = midiPitch;
        ev.data3 = MIDI_STANDARD_VELOCITY;
        writeStream2(&ev, sizeof(ev), NULL);
        *p_ueintx &= ~(1<<txini | 1<<fifocon);
    }

    _outpoint.select();

    if (*p_ueintx & 1<<rxouti)
    {
        MIDIEventPacket ev;
        readStream(&ev, sizeof(ev), NULL);
        
        if (bytesInEndpoint() == 0)
            *p_ueintx &= ~(1<<rxouti | 1<<fifocon); // clear out
    }
}

MIDI::MIDI()
  :
    _inpoint(MIDI_STREAM_IN_EPADDR, EP_TYPE_BULK, MIDI_STREAM_EPSIZE, 1),
    _outpoint(MIDI_STREAM_OUT_EPADDR, EP_TYPE_BULK, MIDI_STREAM_EPSIZE, 1)
{
    *p_usbcon &= ~(1<<otgpade);
    *p_uhwcon |= 1<<uvrege;
    *p_pllfrq = 1<<pdiv2;
    *p_usbcon &= ~(1<<vbuste);
    *p_udien = 0;
    *p_usbint = 0;
    *p_udint = 0;
    *p_usbcon &= ~(1<<usbe);
    *p_usbcon |= 1<<usbe;
    *p_usbcon &= ~(1<<frzclk);
    *p_pllcsr = 0; // pll off
    state = DEVICE_STATE_UNATTACHED;
    confNum = 0;
    _remoteWakeupEn = false;
    USB_Device_CurrentlySelfPowered = false;
    *p_udcon &= ~(1<<lsm); // full speed
    *p_usbcon |= 1<<vbuste;
    configureEndpoint(0, 0, 8, 1);
    *p_udint &= ~(1<<suspi);
    *p_udien |= 1<<suspe;
    *p_udien |= 1<<eorste;
    *p_udcon &= ~(1<<detach);  // attach
    *p_usbcon |= 1<<otgpade;
}

int main()
{
    *p_ddrf &= ~(1<<7 | 1<<6 | 1<<5 | 1<<4 | 1<<1 | 1<<0);
    *p_portf |= 1<<7 | 1<<6 | 1<<5 | 1<<4 | 1<<1 | 1<<0;
    MIDI midi;
    zei();

    while (true)
        midi.task();

    return 0;
}

extern "C" void USB_COM __attribute__ ((signal, used, externally_visible));
void USB_COM
{
    USB::instance->com();
}

extern "C" void USB_GEN __attribute__ ((signal, used, externally_visible));
void USB_GEN
{
    USB::instance->gen();
}


