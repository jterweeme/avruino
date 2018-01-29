/*
Kartonnen piano met aluminium folie toetsen test

eerste test succesvol

D4 is zend pin
*/

#include "busby.h"
#include "usbsound.h"
#include <avr/pgmspace.h>
#include "capsense.h"

static const uint8_t
    MIDI_JACKTYPE_EMBEDDED = 0x01,
    MIDI_JACKTYPE_EXTERNAL = 0x02,
    MIDI_STREAM_IN_EPADDR = ENDPOINT_DIR_IN | 1,
    MIDI_STREAM_OUT_EPADDR = ENDPOINT_DIR_OUT | 2,
    MIDI_STREAM_EPSIZE = 64,
    MIDI_COMMAND_NOTE_ON = 0x90,
    MIDI_COMMAND_NOTE_OFF = 0x80,
    MIDI_STANDARD_VELOCITY = 64,
    INTERFACE_ID_AUDIOCONTROL = 0,
    INTERFACE_ID_AUDIOSTREAM = 1;

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

struct MIDIInterfaceAS
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;
    uint16_t bcdMSC;
    uint16_t wTotalLength;
}
__attribute__ ((packed));

struct MIDIDescInputJack
{
    uint8_t size;
    uint8_t type;
    uint8_t subType;
    uint8_t jackType;
    uint8_t jackID;
    uint8_t jackStrIndex;
}
__attribute__ ((packed));

struct MIDIDescOutputJack
{
    uint8_t size;
    uint8_t type;
    uint8_t subType;
    uint8_t jackType;
    uint8_t jackID;
    uint8_t numPins;
    uint8_t srcJackID[1];
    uint8_t srcPinID[1];
    uint8_t jackStrIndex;
}
__attribute__ ((packed));

struct MIDIEndpoint
{
    uint8_t size;
    uint8_t type;
    uint8_t subtype;
    uint8_t totalEmbeddedJacks;
    uint8_t associatedJackID[1];
}
__attribute__ ((packed));

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

struct MIDIEventPacket
{
    uint8_t event;
    uint8_t data1;
    uint8_t data2;
    uint8_t data3;
}
__attribute__ ((packed));

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

static constexpr uint16_t THRESHOLD = 1500;

class MIDI : public USB
{
private:
    Endpoint _inpoint;
    Endpoint _outpoint;
    CapSense _sense2;
    CapSense _sense6;
    CapSense _sense7;
    CapSense _sense8;
    CapSense _sense9;
    CapSense _sense10;
    CapSense _sense11;
    CapSense _sense12;
    CapSense _senseA2;
    CapSense _senseA3;
    CapSense _senseA4;
    CapSense _senseA5;
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

    uint32_t total2 = _sense2.senseRaw(30);
    uint32_t total6 = _sense6.senseRaw(30);
    uint32_t total7 = _sense7.senseRaw(30);
    uint32_t total8 = _sense8.senseRaw(30);
    uint32_t total9 = _sense9.senseRaw(30);
    uint32_t total10 = _sense10.senseRaw(30);
    uint32_t total11 = _sense11.senseRaw(30);
    uint32_t total12 = _sense12.senseRaw(30);
    uint32_t totalA2 = _senseA2.senseRaw(30);
    uint32_t totalA3 = _senseA3.senseRaw(30);
    uint32_t totalA4 = _senseA4.senseRaw(30);
    uint32_t totalA5 = _senseA5.senseRaw(30);

    if (total2 > THRESHOLD)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3a;
    }

    if (total6 > THRESHOLD)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3b;
    }

    if (total7 > THRESHOLD)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3c;
    }

    if (total8 > THRESHOLD)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3d;
    }

    if (total9 > THRESHOLD)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3e;
    }

    if (total10 > THRESHOLD)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3f;
    }

    if (total11 > THRESHOLD)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x39;
    }

    if (total12 > THRESHOLD)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x38;
    }

    if (totalA2 > THRESHOLD)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x37;
    }

    if (totalA3 > THRESHOLD)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x36;
    }

    if (totalA4 > THRESHOLD)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x35;
    }

    if (totalA5 > 2500)
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x34;
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
    _outpoint(MIDI_STREAM_OUT_EPADDR, EP_TYPE_BULK, MIDI_STREAM_EPSIZE, 1),
    _sense2(p_pin2_base, pin2_bit),
    _sense6(p_pin6_base, pin6_bit),
    _sense7(p_pin7_base, pin7_bit),
    _sense8(p_pin8_base, pin8_bit),
    _sense9(p_pin9_base, pin9_bit),
    _sense10(p_pin10_base, pin10_bit),
    _sense11(p_pin11_base, pin11_bit),
    _sense12(p_pin12_base, pin12_bit),
    _senseA2(p_pinA2_base, pinA2_bit),
    _senseA3(p_pinA3_base, pinA3_bit),
    _senseA4(p_pinA4_base, pinA4_bit),
    _senseA5(p_pinA5_base, pinA5_bit)
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



