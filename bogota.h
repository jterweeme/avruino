#ifndef _BOGOTA_H_
#define _BOGOTA_H_

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

#endif



