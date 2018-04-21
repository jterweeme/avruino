#ifndef _USBMIDI_H_
#define _USBMIDI_H_
#include "busby.h"

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

struct MIDIEventPacket
{
    uint8_t event;
    uint8_t data1;
    uint8_t data2;
    uint8_t data3;
}
__attribute__ ((packed));

#endif



