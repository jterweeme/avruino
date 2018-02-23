#ifndef _BOGOTA_H_
#define _BOGOTA_H_
#include "busby.h"
#include <avr/interrupt.h>
#include <avr/boot.h>

#define SWAPENDIAN_16(x)   (uint16_t)((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8))

#define SWAPENDIAN_32(x) \
  (uint32_t)((((x) & 0xFF000000UL) >> 24UL) | (((x) & 0x00FF0000UL) >> 8UL) | \
                           (((x) & 0x0000FF00UL) << 8UL)  | (((x) & 0x000000FFUL) << 24UL))

#define GCC_MEMORY_BARRIER()                  __asm__ __volatile__("" ::: "memory");
#define GCC_IS_COMPILE_CONST(x)               __builtin_constant_p(x)
#define ATTR_WARN_UNUSED_RESULT      __attribute__ ((warn_unused_result))
#define ATTR_NON_NULL_PTR_ARG(...)   __attribute__ ((nonnull (__VA_ARGS__)))
#define ATTR_PACKED                      __attribute__ ((packed))

static inline uint8_t GetGlobalInterruptMask(void)
{
    GCC_MEMORY_BARRIER();
    return SREG;
}

static inline void SetGlobalInterruptMask(const uint8_t GlobalIntState)
{
    GCC_MEMORY_BARRIER();
    SREG = GlobalIntState;
    GCC_MEMORY_BARRIER();
}

static constexpr uint8_t
    FEATURE_SEL_EndpointHalt = 0x00,
    FEATURE_SEL_TestMode = 0x02,
    USB_CSCP_NoDeviceClass          = 0x00,
    USB_CSCP_NoDeviceSubclass       = 0x00,
    USB_CSCP_NoDeviceProtocol       = 0x00,
    USB_CSCP_VendorSpecificClass    = 0xFF,
    USB_CSCP_VendorSpecificSubclass = 0xFF,
    USB_CSCP_VendorSpecificProtocol = 0xFF,
    USB_CSCP_IADDeviceClass         = 0xEF,
    USB_CSCP_IADDeviceSubclass      = 0x02,
    USB_CSCP_IADDeviceProtocol      = 0x01;

struct USB_Descriptor_Header_t
{
    uint8_t Size;
    uint8_t Type;
} ATTR_PACKED;

typedef struct
{
    uint8_t  Address;
    uint16_t Size;
    uint8_t  Type;
    uint8_t  Banks;
} USB_Endpoint_Table_t;

#define USB_STRING_LEN(UnicodeChars)   (sizeof(USB_Descriptor_Header_t) + ((UnicodeChars) << 1))
#define ARCH_HAS_EEPROM_ADDRESS_SPACE
#define ARCH_HAS_FLASH_ADDRESS_SPACE
#define ARCH_HAS_MULTI_ADDRESS_SPACE
#define ARCH_LITTLE_ENDIAN
#define ENDPOINT_EPNUM_MASK 0x0F

class Busby
{
protected:
    virtual void configure() { }
    virtual void customCtrl() { }
    volatile uint8_t state;
    USBRequest _ctrlReq;
    volatile bool USB_IsInitialized;
    bool USB_Device_CurrentlySelfPowered;
    bool USB_Device_RemoteWakeupEnabled;
public:
    uint8_t USB_Device_ConfigurationNumber;
    static Busby *instance;
    Busby();
    void USB_Init_Device();
    void gen();
    void com();
    //void USB_DeviceTask();
    void USB_Device_GetStatus();
    void procCtrlReq();
    uint8_t readControlStreamLE(void* const Buffer, uint16_t Length);
    uint8_t Endpoint_WaitUntilReady();
    uint8_t write_Control_Stream_LE(const void* const Buffer, uint16_t Length);
    void USB_Device_GetInternalSerialDescriptor();
    void USB_Device_SetAddress();
    void USB_Device_ClearSetFeature();
    uint16_t Endpoint_BytesInEndpoint();
    uint8_t Endpoint_Write_Control_PStream_LE(const void* const Buffer, uint16_t Length);
    uint8_t readStream(void * const buf, uint16_t len, uint16_t* const BytesProcessed);
    //void USB_Init();
    void USB_ResetInterface();
    void Endpoint_ClearStatusStage();
    void USB_Device_GetConfiguration();
    virtual uint16_t getDesc(uint16_t, uint16_t, const void ** const) { return 0; }

    uint8_t Endpoint_Write_PStream_LE(const void * const Buffer,
                            uint16_t Length, uint16_t* const BytesProcessed);

    uint8_t writeStream2(const void * const Buffer, uint16_t Length,
                            uint16_t* const BytesProcessed);

    bool Endpoint_ConfigureEndpointTable(const USB_Endpoint_Table_t* const Table,
                                     const uint8_t Entries);
};

static inline uint8_t Endpoint_BytesToEPSizeMask(const uint16_t Bytes)
{
    uint8_t MaskVal = 0;
    uint16_t CheckBytes = 8;

    while (CheckBytes < Bytes)
    {
        MaskVal++;
        CheckBytes <<= 1;
    }

    return MaskVal << EPSIZE0;
}

static inline void Endpoint_SelectEndpoint(const uint8_t Address)
{
    UENUM = (Address & ENDPOINT_EPNUM_MASK);
}

static bool Endpoint_ConfigureEndpoint_Prv(const uint8_t Number, const uint8_t UECFG0XData,
                                    const uint8_t UECFG1XData)
{
    for (uint8_t EPNum = Number; EPNum < ENDPOINT_TOTAL_ENDPOINTS; EPNum++)
    {
        uint8_t UECFG0XTemp;
        uint8_t UECFG1XTemp;
        uint8_t UEIENXTemp;

        Endpoint_SelectEndpoint(EPNum);

        if (EPNum == Number)
        {
            UECFG0XTemp = UECFG0XData;
            UECFG1XTemp = UECFG1XData;
            UEIENXTemp  = 0;
        }
        else
        {
            UECFG0XTemp = UECFG0X;
            UECFG1XTemp = UECFG1X;
            UEIENXTemp  = UEIENX;
        }

        if (!(UECFG1XTemp & (1 << ALLOC)))
            continue;

        UECONX &= ~(1<<EPEN);
        UECFG1X &= ~(1 << ALLOC);
        UECONX |= 1<<EPEN;
        UECFG0X = UECFG0XTemp;
        UECFG1X = UECFG1XTemp;
        UEIENX = UEIENXTemp;

        if ((UESTA0X & 1<<CFGOK) == 0)
            return false;
    }

    Endpoint_SelectEndpoint(Number);
    return true;
}

static inline bool Endpoint_ConfigureEndpoint(const uint8_t Address,
                         const uint8_t Type, const uint16_t Size, const uint8_t Banks)
{
    uint8_t Number = (Address & ENDPOINT_EPNUM_MASK);

    if (Number >= ENDPOINT_TOTAL_ENDPOINTS)
        return false;

    return Endpoint_ConfigureEndpoint_Prv(Number,
                        ((Type << EPTYPE0) | ((Address & ENDPOINT_DIR_IN) ? (1 << EPDIR) : 0)),
          ((1 << ALLOC) | ((Banks > 1) ? (1 << EPBK0) : 0) | Endpoint_BytesToEPSizeMask(Size)));
}

static inline uint8_t Endpoint_GetEndpointDirection(void)
{
    return (UECFG0X & (1 << EPDIR)) ? ENDPOINT_DIR_IN : ENDPOINT_DIR_OUT;
}

static inline uint8_t Endpoint_GetCurrentEndpoint(void)
{
    return ((UENUM & ENDPOINT_EPNUM_MASK) | Endpoint_GetEndpointDirection());
}

static inline bool Endpoint_IsReadWriteAllowed(void)
{
    return ((UEINTX & (1 << RWAL)) ? true : false);
}

static inline uint8_t Endpoint_GetEndpointInterrupts(void)
{
    return UEINT;
}

static inline bool Endpoint_HasEndpointInterrupted(const uint8_t Address)
{
    return ((Endpoint_GetEndpointInterrupts() &
        (1 << (Address & ENDPOINT_EPNUM_MASK))) ? true : false);
}

static inline void Endpoint_ClearIN(void)
{
    UEINTX &= ~((1 << TXINI) | (1 << FIFOCON));
}

static inline void Endpoint_ClearOUT(void)
{
    UEINTX &= ~((1 << RXOUTI) | (1 << FIFOCON));
}

static inline void Endpoint_SetEndpointDirection(const uint8_t DirectionMask)
{
    UECFG0X = ((UECFG0X & ~(1 << EPDIR)) | (DirectionMask ? (1 << EPDIR) : 0));
}

static inline uint8_t Endpoint_Read_8(void)
{
    return UEDATX;
}

static inline void Endpoint_Write_8(const uint8_t Data)
{
    UEDATX = Data;
}

static inline void Endpoint_Discard_8(void)
{
    uint8_t Dummy;
    Dummy = UEDATX;
    (void)Dummy;
}

static inline uint16_t Endpoint_Read_16_LE(void)
{
    union
    {
        uint16_t Value;
        uint8_t  Bytes[2];
    } Data;

    Data.Bytes[0] = UEDATX;
    Data.Bytes[1] = UEDATX;
    return Data.Value;
}

static inline uint16_t Endpoint_Read_16_BE(void)
{
    union
    {
        uint16_t Value;
        uint8_t  Bytes[2];
    } Data;

    Data.Bytes[1] = UEDATX;
    Data.Bytes[0] = UEDATX;
    return Data.Value;
}

static inline void Endpoint_Write_16_LE(const uint16_t Data)
{
    UEDATX = (Data & 0xFF);
    UEDATX = (Data >> 8);
}

static inline void Endpoint_Write_16_BE(const uint16_t Data)
{
    UEDATX = (Data >> 8);
    UEDATX = (Data & 0xFF);
}

static inline void Endpoint_Discard_16(void)
{
    uint8_t Dummy;
    Dummy = UEDATX;
    Dummy = UEDATX;
    (void)Dummy;
}

static inline uint32_t Endpoint_Read_32_LE(void)
{
    union { uint32_t Value; uint8_t  Bytes[4]; } Data;
    Data.Bytes[0] = UEDATX;
    Data.Bytes[1] = UEDATX;
    Data.Bytes[2] = UEDATX;
    Data.Bytes[3] = UEDATX;
    return Data.Value;
}

static inline uint32_t Endpoint_Read_32_BE(void)
{
    union
    {
        uint32_t Value;
        uint8_t  Bytes[4];
    } Data;

    Data.Bytes[3] = UEDATX;
    Data.Bytes[2] = UEDATX;
    Data.Bytes[1] = UEDATX;
    Data.Bytes[0] = UEDATX;
    return Data.Value;
}

static inline void Endpoint_Write_32_LE(const uint32_t Data)
{
    UEDATX = (Data &  0xFF);
    UEDATX = (Data >> 8);
    UEDATX = (Data >> 16);
    UEDATX = (Data >> 24);
}

static inline void Endpoint_Write_32_BE(const uint32_t Data)
{
    UEDATX = (Data >> 24);
    UEDATX = (Data >> 16);
    UEDATX = (Data >> 8);
    UEDATX = (Data &  0xFF);
}

static inline void Endpoint_Discard_32(void)
{
    uint8_t Dummy;
    Dummy = UEDATX;
    Dummy = UEDATX;
    Dummy = UEDATX;
    Dummy = UEDATX;
    (void)Dummy;
}

#define FIXED_CONTROL_ENDPOINT_SIZE      8
#define USB_Device_ControlEndpointSize FIXED_CONTROL_ENDPOINT_SIZE

#define USB_DEVICE_OPT_LOWSPEED            (1 << 0)
#define USB_DEVICE_OPT_FULLSPEED               (0 << 0)

#define USE_INTERNAL_SERIAL            0xDC
#define INTERNAL_SERIAL_LENGTH_BITS    80
#define INTERNAL_SERIAL_START_ADDRESS  0x0E

static inline uint16_t USB_Device_GetFrameNumber(void)
{
    return UDFNUM;
}

static inline void USB_Device_EnableSOFEvents(void)
{
    UDIEN |= 1<<SOFE;
}
    
static inline void USB_Device_DisableSOFEvents(void)
{   
    UDIEN &= ~(1<<SOFE);
}

static inline void USB_Device_SetDeviceAddress(const uint8_t Address)
{
    UDADDR = (UDADDR & (1 << ADDEN)) | (Address & 0x7F);
}

static inline void USB_Device_EnableDeviceAddress(const uint8_t Address)
{
    (void)Address;
    UDADDR |= 1<<ADDEN;
}

static inline void USB_Device_GetSerialString(uint16_t* const UnicodeString)
{
    uint8_t CurrentGlobalInt = GetGlobalInterruptMask();
    sei();
    uint8_t SigReadAddress = INTERNAL_SERIAL_START_ADDRESS;

    for (uint8_t SerialCharNum = 0; SerialCharNum < (INTERNAL_SERIAL_LENGTH_BITS / 4);
        SerialCharNum++)
    {
        uint8_t SerialByte = boot_signature_byte_get(SigReadAddress);

        if (SerialCharNum & 0x01)
        {
            SerialByte >>= 4;
            SigReadAddress++;
        }

        SerialByte &= 0x0F;

        UnicodeString[SerialCharNum] = (SerialByte >= 10) ?
                (('A' - 10) + SerialByte) : ('0' + SerialByte);
    }

    SetGlobalInterruptMask(CurrentGlobalInt);
}

static constexpr uint8_t
    MEMSPACE_FLASH = 0,
    MEMSPACE_EEPROM = 1,
    MEMSPACE_RAM = 2;

#define USB_PLL_PSC                (1 << PINDIV)
#define USB_OPT_REG_DISABLED               (1 << 1)
#define USB_OPT_REG_ENABLED                (0 << 1)
#define USB_OPT_REG_KEEP_ENABLED           (1 << 3)
#define USB_OPT_MANUAL_PLL                 (1 << 2)
#define USB_OPT_AUTO_PLL                   (0 << 2)
#define USB_STREAM_TIMEOUT_MS       100

static inline bool USB_VBUS_GetStatus(void)
{
    return ((USBSTA & (1 << VBUS)) ? true : false);
}

#define USE_STATIC_OPTIONS (USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)
#define USB_Options USE_STATIC_OPTIONS

static inline bool USB_PLL_IsReady(void)
{
    return ((PLLCSR & (1 << PLOCK)) ? true : false);
}

static inline void USB_OTGPAD_Off(void)
{
    USBCON &= ~(1 << OTGPADE);
}

static inline void USB_CLK_Freeze(void)
{
    USBCON |= 1<<FRZCLK;
}

static inline void USB_Controller_Enable(void)
{
    USBCON |= 1<<USBE;
}

static inline void USB_Controller_Disable(void)
{
    USBCON &= ~(1<<USBE);
}

static inline void USB_Controller_Reset(void)
{
    USBCON &= ~(1 << USBE);
    USBCON |=  (1 << USBE);
}

static inline bool Endpoint_IsSETUPReceived(void)
{
    return ((UEINTX & (1 << RXSTPI)) ? true : false);
}

static inline bool Endpoint_IsOUTReceived(void)
{
    return ((UEINTX & (1 << RXOUTI)) ? true : false);
}

static inline bool USB_Device_IsAddressSet(void)
{
    return UDADDR & 1<<ADDEN;
}

static inline bool Endpoint_IsStalled(void)
{
    return UECONX & 1<<STALLRQ ? true : false;
}

#define USB_HOST_TIMEOUT_MS                1000

#endif



