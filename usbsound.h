#ifndef _USBSOUND_H_
#define _USBSOUND_H_

#ifndef ATTR_PACKED
#define ATTR_PACKED __attribute__ ((packed))
#endif

static constexpr uint16_t
    AUDIO_STREAM_EPSIZE = 256,
    AUDIO_TERMINAL_UNDEFINED = 0x0100,
    AUDIO_TERMINAL_STREAMING = 0x0101,
    AUDIO_TERMINAL_VENDOR = 0x01FF,
    AUDIO_TERMINAL_IN_UNDEFINED = 0x0200,
    AUDIO_TERMINAL_OUT_SPEAKER = 0x0301,
    AUDIO_TERMINAL_OUT_HEADPHONES = 0x0302,
    AUDIO_TERMINAL_OUT_HEAD_MOUNTED = 0x0303;

static constexpr uint8_t
    AUDIO_STREAM_EPADDR = ENDPOINT_DIR_OUT | 1,
    AUDIO_CHANNEL_LEFT_FRONT = 1<<0,
    AUDIO_CHANNEL_RIGHT_FRONT = 1<<1,
    AUDIO_EP_SAMPLE_FREQ_CONTROL = 1<<0,
    AUDIO_EP_PITCH_CONTROL = 1<<1,
    AUDIO_EP_FULL_PACKETS_ONLY = 1<<7,
    AUDIO_EP_ACCEPTS_SMALL_PACKETS = 0<<7,
    AUDIO_DSUBTYPE_CSInterface_General = 0x01,
    AUDIO_DSUBTYPE_CSInterface_FormatType = 0x02,
    AUDIO_DSUBTYPE_CSInterface_FormatSpecific = 0x03,
    AUDIO_DSUBTYPE_CSEndpoint_General = 0x01,
    AUDIO_CSCP_AUDIOCLASS = 0x01,
    AUDIO_CSCP_CONTROLSUBCLASS = 0x01,
    AUDIO_CSCP_CONTROLPROTOCOL = 0x00,
    AUDIO_CSCP_AUDIOSTREAMINGSUBCLASS = 0x02,
    AUDIO_CSCP_MIDISTREAMINGSUBCLASS = 0x03,
    AUDIO_CSCP_STREAMINGPROTOCOL = 0x00,
    AUDIO_DSUBTYPE_CSENDPOINT_GENERAL = 0x01,
    AUDIO_DSUBTYPE_CSINTERFACE_HEADER = 0x01,
    AUDIO_DSUBTYPE_CSINTERFACE_INPUTTERMINAL = 0x02,
    AUDIO_DSUBTYPE_CSINTERFACE_OUTPUTTERMINAL = 0x03,
    AUDIO_REQ_SETCURRENT = 0x01,
    AUDIO_REQ_GETCURRENT = 0x81,
    AUDIO_REQ_GETSTATUS = 0xFF,
    AUDIO_EPCONTROL_SamplingFreq = 0x01,
    AUDIO_EPCONTROL_Pitch = 0x02,
    INTERFACE_ID_AudioControl = 0,
    INTERFACE_ID_AudioStream = 1;

struct DescAudioInTerminal
{
    uint8_t size;
    uint8_t type;
    uint8_t subtype;
    uint8_t terminalID;
    uint16_t terminalType;
    uint8_t associatedOutputTerminal;
    uint8_t totalChannels;
    uint16_t channelConfig;
    uint8_t channelStrIndex;
    uint8_t terminalStrIndex;
} ATTR_PACKED;

struct DescAudioOutTerminal
{
    uint8_t size;
    uint8_t type;
    uint8_t subtype;
    uint8_t terminalID;
    uint16_t terminalType;
    uint8_t associatedInputTerminal;
    uint8_t sourceID;
    uint8_t terminalStrIndex;
} ATTR_PACKED;

struct DescAudioIface
{
    uint8_t size;
    uint8_t type;
    uint8_t Subtype;
    uint16_t ACSpecification;
    uint16_t TotalLength;
    uint8_t InCollection;
    uint8_t InterfaceNumber;
} ATTR_PACKED;

struct SampleFreq
{
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
} ATTR_PACKED;

struct AudioEndpoint
{
    DescEndpoint endpoint;
    uint8_t Refresh;
    uint8_t SyncEndpointNumber;
} ATTR_PACKED;

struct AudioEndpointSpc
{
    uint8_t size;
    uint8_t type;
    uint8_t subtype;
    uint8_t attributes;
    uint8_t lockDelayUnits;
    uint16_t lockDelay;
}
ATTR_PACKED;

struct DescAudioIfaceAS
{
    uint8_t size;
    uint8_t type;
    uint8_t subtype;
    uint8_t terminalLink;
    uint8_t frameDelay;
    uint16_t audioFormat;
}
__attribute__ ((packed));

struct AudioFormat
{
    uint8_t size;
    uint8_t type;
    uint8_t Subtype;
    uint8_t FormatType;
    uint8_t Channels;
    uint8_t SubFrameSize;
    uint8_t BitResolution;
    uint8_t TotalDiscreteSampleRates;
} ATTR_PACKED;

struct USB_Descriptor_Configuration_t
{
    DescConf Config;
    DescIface Audio_ControlInterface;
    DescAudioIface Audio_ControlInterface_SPC;
    DescAudioInTerminal Audio_InputTerminal;
    DescAudioOutTerminal Audio_OutputTerminal;
    DescIface Audio_StreamInterface_Alt0;
    DescIface Audio_StreamInterface_Alt1;
    DescAudioIfaceAS Audio_StreamInterface_SPC;
    AudioFormat Audio_AudioFormat;
    SampleFreq Audio_AudioFormatSampleRates[5];
    AudioEndpoint Audio_StreamEndpoint;
    AudioEndpointSpc Audio_StreamEndpoint_SPC;
} ATTR_PACKED;





#endif



