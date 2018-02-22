/*
Kartonnen piano met aluminium folie toetsen test

eerste test succesvol

D13 is zend pin
*/

#include "busby.h"
#include "usbsound.h"
#include <avr/pgmspace.h>
#include "capsense.h"
#include "usbmidi.h"

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

static constexpr uint16_t THRESHOLD = 3000;

class MIDI : public USB
{
private:
    Board _board;
    Endpoint _inpoint;
    Endpoint _outpoint;
    CapSense _toets01;
    CapSense _toets02;
    CapSense _toets03;
    CapSense _toets04;
    CapSense _toets05;
    CapSense _toets06;
    CapSense _toets07;
    CapSense _toets08;
    CapSense _toets09;
    CapSense _toets10;
    CapSense _toets11;
    CapSense _toets12;
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
    uint32_t toets01 = _toets01.senseRaw(30);
    uint32_t toets02 = _toets02.senseRaw(30);
    uint32_t toets03 = _toets03.senseRaw(30);
    uint32_t toets04 = _toets04.senseRaw(30);
    uint32_t toets05 = _toets05.senseRaw(30);
    uint32_t toets06 = _toets06.senseRaw(30);
    uint32_t toets07 = _toets07.senseRaw(30);
    uint32_t toets08 = _toets08.senseRaw(30);
    uint32_t toets09 = _toets09.senseRaw(30);
    uint32_t toets10 = _toets10.senseRaw(30);
    uint32_t toets11 = _toets11.senseRaw(30);
    uint32_t toets12 = _toets12.senseRaw(30);

    if (toets01 > _toets01.threshold())
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x30;
    }

    if (toets02 > _toets02.threshold())
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x33;
    }

    if (toets03 > _toets03.threshold())
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x37;
    }

    if (toets04 > _toets04.threshold())
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x38;
    }

    if (toets05 > _toets05.threshold())
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x39;
    }

    if (toets06 > _toets06.threshold())
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3a;
    }

    if (toets07 > _toets07.threshold())
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3b;
    }

    if (toets08 > _toets08.threshold())
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3c;
    }

    if (toets09 > _toets09.threshold())
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3d;
    }

    if (toets10 > _toets10.threshold())
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3e;
    }

    if (toets11 > _toets11.threshold())
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x3f;
    }

    if (toets12 > _toets12.threshold())
    {
        midiCommand = MIDI_COMMAND_NOTE_ON;
        midiPitch = 0x46;
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
    _toets01(_board.pin13, _board.pin12, 1500),
    _toets02(_board.pin13, _board.pin11, 1500),
    _toets03(_board.pin13, _board.pin10, 1500),
    _toets04(_board.pin13, _board.pin9, 1500),
    _toets05(_board.pin13, _board.pin8, 1500),
    _toets06(_board.pin13, _board.pin7, 1500),
    _toets07(_board.pin13, _board.pin6, 1500),
    _toets08(_board.pin13, _board.pin5, 1500),
    _toets09(_board.pin13, _board.pinA2, 1500),
    _toets10(_board.pin13, _board.pinA3, 2000),
    _toets11(_board.pin13, _board.pinA4, 1500),
    _toets12(_board.pin13, _board.pinA5, 2200)
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

#ifndef BUSBY_INT
extern "C" void __vector_11() __attribute__ ((signal, used, externally_visible));
void __vector_11()
{
    USB::instance->com();
}

extern "C" void __vector_10() __attribute__ ((signal, used, externally_visible));
void __vector_10()
{
    USB::instance->gen();
}
#endif



