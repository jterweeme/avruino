#include "usbsd.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <ctype.h>

static constexpr uint8_t
    E2P_TYPE_BULK = 0x02,
    UZE_INTERNAL_SERIAL = 0xdc,
    USB_CONFYG_ATTR_RESERVED = 0x80,
    MS_KOMMAND_DIR_DATA_IN = 1<<7,
    ZCZI_ASENSE_NO_ADDITIONAL_INFORMATION = 0x00,
    ZCZI_ASENSEQ_NO_QUALIFIER = 0x00,
    ZCZI_SENSE_KEY_GOOD = 0x00,
    ZCZI_SENSE_KEY_HARDWARE_ERROR = 0x04,
    ZCZI_SENSE_KEY_ILLEGAL_REQUEST = 0x05,
    ZCZI_SENSE_KEY_DATA_PROTECT = 0x07,
    ZCZI_ASENSE_INVALID_COMMAND = 0x20,
    ZCZI_ASENSE_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE = 0x21,
    ZCZI_ASENSE_INVALID_FIELD_IN_CDB = 0x24,
    ZCZI_ASENSE_WRITE_PROTECTED = 0x27,
    ZCZI_CMD_TEST_UNIT_READY = 0x00,
    ZCZI_CMD_REQUEST_SENSE = 0x03,
    ZCZI_CMD_INQUIRY = 0x12,
    ZCZI_CMD_MODE_SENSE_6 = 0x1a,
    ZCZI_CMD_START_STOP_UNIT = 0x1b,
    ZCZI_CMD_SEND_DIAGNOSTIC = 0x1d,
    ZCZI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL = 0x1e,
    ZCZI_CMD_READ_CAPACITY_10 = 0x25,
    ZCZI_CMD_READ_10 = 0x28,
    ZCZI_CMD_WRITE_10 = 0x2a,
    ZCZI_CMD_VERIFY_10 = 0x2f,
    MS_ZCZI_COMMAND_Pass = 0,
    MS_ZCZI_COMMAND_Fail = 1,
    MS_CZCP_SCSITransparentSubclass = 0x06,
    MS_CZCP_MassStorageClass = 0x08,
    MS_CZCP_BulkOnlyTransportProtocol = 0x50,
    STRING_ID_Language     = 0,
    STRING_ID_Manufacturer = 1,
    STRING_ID_Product      = 2,
    MASS_STORAGE_IO_EPSIZE = 64,
    M2S_REQ_GetMaxLUN = 0xfe,
    M2S_REQ_MassStorageReset = 0xff,
    INTERNAL_ZERIAL_START_ADDRESS = 0x0e,
    INTERNAL_ZERIAL_LENGTH_BITS = 80,
    MASS_STORAGE_IN_EPADDR = (ENDPOINT_DIR_IN  | 3),
    MASS_STORAGE_OUT_EPADDR = (ENDPOINT_DIR_OUT | 4),
    TOTAL_LUNS = 1;

struct ZCZI_Inquiry_Response_t
{
    uint8_t byte1;
    uint8_t byte2;
    uint8_t version;
    uint8_t byte4;
    uint8_t additionalLen;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t byte8;
    char VendorID[8];
    char ProductID[16];
    char RevisionID[4];
}
__attribute__ ((packed));

struct ZCZI_Request_Sense_Response_t
{
    uint8_t ResponseCode;
    uint8_t SegmentNumber;
    uint8_t vanalles;
    uint8_t Information[4];
    uint8_t AdditionalLength;
    uint8_t CmdSpecificInformation[4];
    uint8_t AdditionalSenseCode;
    uint8_t AdditionalSenseQualifier;
    uint8_t FieldReplaceableUnitCode;
    uint8_t SenseKeySpecific[3];
}
__attribute__ ((packed));

struct MyConf
{
    DescConf Config;
    DescIface MS_Interface;
    DescEndpoint MS_DataInEndpoint;
    DescEndpoint MS_DataOutEndpoint;
};

static constexpr uint16_t BLOCKSIZE = 512;

template <class T> const T& min(const T& a, const T& b) { return !(b < a) ? a : b; }

static inline uint16_t zwapEndian_16(uint16_t word)
{
    return word >> 8 | word << 8;
}

static inline uint32_t zwapEndian_32(const uint32_t dword)
{
    return (dword >> 24 & 0xff) | (dword << 8 & 0xff0000) |
        (dword >> 8 & 0xff00) | (dword << 24 & 0xff000000);
}

static const DescDev PROGMEM DeviceDescriptor =
{
    sizeof(DescDev),
    DTYPE_Device,
    0x0110,
    0,
    0,
    0,
    8,
    0x03EB,
    0x2045,
    0x0001,
    STRING_ID_Manufacturer,
    STRING_ID_Product,
    UZE_INTERNAL_SERIAL,
    FIXED_NUM_CONFIGURATIONS
};


volatile bool IsMassStoreReset = false;

static constexpr bool DATA_READ = true, DATA_WRITE = false;
static constexpr uint8_t DEVICE_TYPE_BLOCK = 0;

static const MyConf PROGMEM myConf =
{
    {
        sizeof(DescConf),
        DTYPE_Configuration,
        sizeof(MyConf),
        1,
        1,
        0,    // no descriptor
        USB_CONFYG_ATTR_RESERVED,
        USB_CONFIG_POWER_MA(100)
    },
    {
        sizeof(DescIface),
        DTYPE_Interface,
        0, // mass storage
        0,
        2,
        MS_CZCP_MassStorageClass,
        MS_CZCP_SCSITransparentSubclass,
        MS_CZCP_BulkOnlyTransportProtocol,
        0
    },
    {
        sizeof(DescEndpoint),
        DTYPE_Endpoint,
        MASS_STORAGE_IN_EPADDR,
        E2P_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        MASS_STORAGE_IO_EPSIZE,
        0x05
    },
    {
        sizeof(DescEndpoint),
        DTYPE_Endpoint,
        MASS_STORAGE_OUT_EPADDR,
        E2P_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        MASS_STORAGE_IO_EPSIZE,
        0x05
    }
};

static const DescString<2> PROGMEM languageString =
{
    USB_STRING_LEN(1),
    DTYPE_String,
    (wchar_t)0x0409
};

static const DescString<12> PROGMEM manufacturerString =
{
    USB_STRING_LEN(11),
    DTYPE_String,
    L"Dean Camera"
};

static const DescString<23> PROGMEM productString =
{
    USB_STRING_LEN(22),
    DTYPE_String,
    L"LUFA Mass Storage Demo"
};

static uint16_t getDescriptor(uint16_t wValue, uint16_t wIndex, const void** const descAddr)
{
    const void *addr = NULL;
    uint16_t size = 0;

    switch (wValue >> 8)
    {
    case DTYPE_Device:
        addr = &DeviceDescriptor;
        size = sizeof(DescDev);
        break;
    case DTYPE_Configuration:
        addr = &myConf;
        size = sizeof(MyConf);
        break;
    case DTYPE_String:
        switch (wValue & 0xff)
        {
        case STRING_ID_Language:
            addr = &languageString;
            size = pgm_read_byte(&languageString.size);
            break;
        case STRING_ID_Manufacturer:
            addr = &manufacturerString;
            size = pgm_read_byte(&manufacturerString.size);
            break;
        case STRING_ID_Product:
            addr = &productString;
            size = pgm_read_byte(&productString.size);
            break;
        }
        break;
    }

    *descAddr = addr;
    return size;
}

static const ZCZI_Inquiry_Response_t InquiryData =
{
    DEVICE_TYPE_BLOCK<<3 | 0,
    1,  // removable
    0,  // version
    2<<4,
    0x1f,   // additionalLength
    0,
    0,  // reserved1
    0,  // reserved2
    "LUFA",
    "Dataflash Disk",
    {'0','.','0','0'},
};

static ZCZI_Request_Sense_Response_t senseData =
{
    0x70,   // response code
    0, // seg no
    0, // van alles
    {0,0,0,0},
    0x0A
};

bool USBSD::decodeSCSICmd()
{
    bool success = false;

    switch (cmdBlock.SCSICommandData[0])
    {
    case ZCZI_CMD_INQUIRY:
    {
        uint16_t allocLen = zwapEndian_16(*(uint16_t*)&cmdBlock.SCSICommandData[3]);
        uint16_t BytesTransferred = min(allocLen, sizeof(InquiryData));

        if ((cmdBlock.SCSICommandData[1] & (1<<0 | 1<<1)) || cmdBlock.SCSICommandData[2])
        {
            senseData.vanalles = ZCZI_SENSE_KEY_ILLEGAL_REQUEST << 4;
            senseData.AdditionalSenseCode = ZCZI_ASENSE_INVALID_FIELD_IN_CDB;
            senseData.AdditionalSenseQualifier = ZCZI_ASENSEQ_NO_QUALIFIER;
            break;
        }

        writeStream2(&InquiryData, BytesTransferred, NULL);
        nullStream(allocLen - BytesTransferred, NULL);
        UEINTX &= ~(1<<TXINI | 1<<FIFOCON); // clear in
        cmdBlock.DataTransferLength -= BytesTransferred;
        success = true;
    }
        break;
    case ZCZI_CMD_REQUEST_SENSE:
    {
        uint8_t AllocationLength = cmdBlock.SCSICommandData[4];
        uint8_t BytesTransferred = min((size_t)AllocationLength, sizeof(senseData));
        writeStream2(&senseData, BytesTransferred, NULL);
        nullStream(AllocationLength - BytesTransferred, NULL);
        UEINTX &= ~(1<<TXINI | 1<<FIFOCON); // clear in
        cmdBlock.DataTransferLength -= BytesTransferred;
        success = true;
    }
        break;
    case ZCZI_CMD_READ_CAPACITY_10:
        write32be(_blocks - 1);
        write32be(BLOCKSIZE);

        if (IsMassStoreReset)
            break;

        *p_ueintx &= ~(1<<txini | 1<<fifocon);
        cmdBlock.DataTransferLength -= 8;
        success = true;
        break;
    case ZCZI_CMD_SEND_DIAGNOSTIC:

        if ((cmdBlock.SCSICommandData[1] & 1<<2) == 0)
        {
            senseData.vanalles = ZCZI_SENSE_KEY_ILLEGAL_REQUEST << 4;
            senseData.AdditionalSenseCode = ZCZI_ASENSE_INVALID_FIELD_IN_CDB;
            senseData.AdditionalSenseQualifier = ZCZI_ASENSEQ_NO_QUALIFIER;
            break;
        }

        cmdBlock.DataTransferLength = 0;
        success = true;
        break;
    case ZCZI_CMD_WRITE_10:
        success = SCSI_Command_ReadWrite_10(DATA_WRITE);
        break;
    case ZCZI_CMD_READ_10:
        success = SCSI_Command_ReadWrite_10(DATA_READ);
        break;
    case ZCZI_CMD_MODE_SENSE_6:
        write8(0x00);
        write8(0x00);
        write8(0x00);
        write8(0x00);
        UEINTX &= ~(1<<TXINI | 1<<FIFOCON); // clear in
        cmdBlock.DataTransferLength -= 4;
        success = true;
        break;
    case ZCZI_CMD_START_STOP_UNIT:
    case ZCZI_CMD_TEST_UNIT_READY:
    case ZCZI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
    case ZCZI_CMD_VERIFY_10:
        success = true;
        cmdBlock.DataTransferLength = 0;
        break;
    default:
        senseData.vanalles = ZCZI_SENSE_KEY_ILLEGAL_REQUEST << 4;
        senseData.AdditionalSenseCode = ZCZI_ASENSE_INVALID_COMMAND;
        senseData.AdditionalSenseQualifier = ZCZI_ASENSEQ_NO_QUALIFIER;
        break;
    }

    if (success)
    {
        senseData.vanalles = ZCZI_SENSE_KEY_GOOD << 4;
        senseData.AdditionalSenseCode = ZCZI_ASENSE_NO_ADDITIONAL_INFORMATION;
        senseData.AdditionalSenseQualifier = ZCZI_ASENSEQ_NO_QUALIFIER;
        return true;
    }

    return false;
}

bool USBSD::SCSI_Command_ReadWrite_10(bool IsDataRead)
{
    uint32_t blockAddr = zwapEndian_32(*(uint32_t*)&cmdBlock.SCSICommandData[2]);
    uint32_t totalBlocks = zwapEndian_16(*(uint16_t*)&cmdBlock.SCSICommandData[7]);

    if (blockAddr >= _blocks)
    {
        senseData.vanalles = ZCZI_SENSE_KEY_ILLEGAL_REQUEST << 4;
        senseData.AdditionalSenseCode = ZCZI_ASENSE_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE;
        senseData.AdditionalSenseQualifier = ZCZI_ASENSEQ_NO_QUALIFIER;
        return false;
    }

    if (IsDataRead == DATA_READ)
        _sdReadBlocks(blockAddr, totalBlocks);
    else
        _sdWriteBlocks(blockAddr, totalBlocks);

    cmdBlock.DataTransferLength -= ((uint32_t)totalBlocks * BLOCKSIZE);
    return true;
}

inline void USBSD::_sdWriteBlock(const uint32_t block)
{
    uint8_t buf[512];
    
    for (uint16_t byte = 0; byte < BLOCKSIZE; )
    {
        if ((*p_ueintx & 1<<rwal) == 0)
        {
            UEINTX &= ~(1<<RXOUTI | 1<<FIFOCON);
            
            if (waitUntilReady())
                return;
        }

        for (uint8_t i = 0; i < 16; i++)
            buf[byte++] = read8();
    }

    _sd.writeBlock(block, buf);
}

void USBSD::_sdWriteBlocks(const uint32_t start, const uint16_t n)
{
    if (waitUntilReady())
        return;

    for (uint16_t i = 0; i < n; i++)
        _sdWriteBlock(start + i);

    if ((UEINTX & 1<<RWAL) == 0)
        UEINTX &= ~(1<<RXOUTI | 1<<FIFOCON);
}

inline void USBSD::_sdReadBlock(const uint32_t block)
{
    uint8_t buf[512];
    _sd.readBlock(block, buf);

    for (uint16_t byte = 0; byte < BLOCKSIZE; )
    {
        if ((UEINTX & 1<<RWAL) == 0)    //read-write allowed?
        {
            UEINTX &= ~(1<<TXINI | 1<<FIFOCON); // clear in

            if (waitUntilReady())
                return;
        }

        for (uint8_t i = 0; i < 16; i++)
            write8(buf[byte++]);

        if (IsMassStoreReset)
            return;
    }
}

void USBSD::_sdReadBlocks(const uint32_t start, const uint16_t n)
{
    if (waitUntilReady())
        return;

    for (uint16_t i = 0; i < n; i++)
        _sdReadBlock(start + i);

    if ((UEINTX & 1<<RWAL) == 0)            // buffer full?
        UEINTX &= ~(1<<TXINI | 1<<FIFOCON); // then clear it
}

void USBSD::MassStorage_Task()
{
	if (state != DEVICE_STATE_Configured)
	    return;

    if (ReadInCommandBlock())
    {
        if (cmdBlock.Flags & MS_KOMMAND_DIR_DATA_IN)
            selectEndpoint(MASS_STORAGE_IN_EPADDR);

		cmdStatus.Status = decodeSCSICmd() ? MS_ZCZI_COMMAND_Pass : MS_ZCZI_COMMAND_Fail;
		cmdStatus.Tag = cmdBlock.Tag;
		cmdStatus.DataTransferResidue = cmdBlock.DataTransferLength;

		if ((cmdStatus.Status == MS_ZCZI_COMMAND_Fail) && cmdStatus.DataTransferResidue)
		    UECONX |= 1<<STALLRQ;   // stall transaction

		ReturnCommandStatus();
	}

	if (IsMassStoreReset)
	{
        _outpoint.reset();
        _inpoint.reset();
        selectEndpoint(MASS_STORAGE_OUT_EPADDR);
        UECONX |= 1<<STALLRQC;  // clear stall
        UECONX |= 1<<RSTDT; // reset data toggle
		selectEndpoint(MASS_STORAGE_IN_EPADDR);
        UECONX |= 1<<STALLRQC;  // clear stall
        UECONX |= 1<<RSTDT; // reset data toggle
		IsMassStoreReset = false;
	}
}

bool USBSD::ReadInCommandBlock()
{
	uint16_t BytesTransferred;
	selectEndpoint(MASS_STORAGE_OUT_EPADDR);

	if ((UEINTX & 1<<RXOUTI) == 0)  // is out received?
	    return false;

	BytesTransferred = 0;

	while (readStream(&cmdBlock, (sizeof(cmdBlock) -
        sizeof(cmdBlock.SCSICommandData)),
	    &BytesTransferred) == ENDPOINT_RWSTREAM_IncompleteTransfer)
	{
		if (IsMassStoreReset)
		    return false;
	}

	if (cmdBlock.Signature != M2S_CBW_SIGNATURE || cmdBlock.LUN >= TOTAL_LUNS ||
        cmdBlock.Flags & 0x1F || cmdBlock.SCSICommandLength == 0 ||
		(cmdBlock.SCSICommandLength > sizeof(cmdBlock.SCSICommandData)))
	{
        *p_ueconx |= 1<<STALLRQ;
		selectEndpoint(MASS_STORAGE_IN_EPADDR);
        *p_ueconx |= 1<<STALLRQ;       // stall transaction
		return false;
	}

	BytesTransferred = 0;

	while (readStream(&cmdBlock.SCSICommandData, cmdBlock.SCSICommandLength,
	                               &BytesTransferred) == ENDPOINT_RWSTREAM_IncompleteTransfer)
	{
		if (IsMassStoreReset)
		    return false;
	}

    UEINTX &= ~(1<<RXOUTI | 1<<FIFOCON);    // clear out
	return true;
}

void USBSD::ReturnCommandStatus()
{
	uint16_t BytesTransferred;
	selectEndpoint(MASS_STORAGE_OUT_EPADDR);

	while (*p_ueconx & 1<<stallrq)
		if (IsMassStoreReset)
		    return;

	selectEndpoint(MASS_STORAGE_IN_EPADDR);

	while (*p_ueconx & 1<<stallrq)
		if (IsMassStoreReset)
		    return;

	BytesTransferred = 0;

	while (writeStream2(&cmdStatus, sizeof(cmdStatus),
	                                &BytesTransferred) == ENDPOINT_RWSTREAM_IncompleteTransfer)
	{
		if (IsMassStoreReset)
		    return;
	}

    UEINTX &= ~(1<<TXINI | 1<<FIFOCON); // clear in
}

void USBSD::procCtrlReq()
{
    uint8_t* RequestHeader = (uint8_t*)&_ctrlReq;

    for (uint8_t i = 0; i < sizeof(USBRequest); i++)
        *(RequestHeader++) = read8();

    if (*p_ueintx & 1<<rxstpi)
    {
        const uint8_t bmRequestType = _ctrlReq.bmRequestType;

        switch (_ctrlReq.bRequest)
        {
        case M2S_REQ_MassStorageReset:
            if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                *p_ueintx &= ~(1<<rxstpi); // clear setup
                clearStatusStage();
                IsMassStoreReset = true;
            }

            break;
        case M2S_REQ_GetMaxLUN:
            if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                UEINTX &= ~(1<<RXSTPI); // clear setup
                write8(TOTAL_LUNS - 1);
                UEINTX &= ~(1<<TXINI | 1<<FIFOCON); // clear in
                clearStatusStage();
            }
            break;
        case REQ_GetStatus:
            if ((bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE)) ||
                (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_ENDPOINT)))
            {
                uint8_t CurrentStatus = 0;

                switch (bmRequestType)
                {
                case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE):
                {
                    if (USB_Device_CurrentlySelfPowered)
                        CurrentStatus |= FEATURE_SELFPOWERED_ENABLED;

                    if (USB_Device_RemoteWakeupEnabled)
                        CurrentStatus |= FEATURE_REMOTE_WAKEUP_ENABLED;
                }
                    break;
                case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_ENDPOINT):
                {
                    uint8_t endpointIndex = ((uint8_t)_ctrlReq.wIndex & ENDPOINT_EPNUM_MASK);

                    if (endpointIndex >= ENDPOINT_TOTAL_ENDPOINTS)
                        return;

                    selectEndpoint(endpointIndex);
                    CurrentStatus = UECONX & 1<<STALLRQ;
                    _control.select();
                }
                    break;
                default:
                    return;
                }

                UEINTX &= ~(1<<RXSTPI); // clear setup
                write16(CurrentStatus);
                UEINTX &= ~(1<<TXINI | 1<<FIFOCON); // clear in
                clearStatusStage();
            }

            break;
        case REQ_ClearFeature:
        case REQ_SetFeature:
            if ((bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE)) ||
                (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_ENDPOINT)))
            {
                Device_ClearSetFeature();
            }

            break;
        case REQ_SetAddress:
            if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
            {
                uint8_t DeviceAddress = _ctrlReq.wValue & 0x7F;
                setDevAddr(DeviceAddress);
                *p_ueintx &= ~(1<<rxstpi); // clear setup
                clearStatusStage();
                while ((*p_ueintx & 1<<txini) == 0);   // in ready?
                *p_udaddr |= 1<<adden; // enable dev addr
                state = DeviceAddress ? DEVICE_STATE_Addressed : DEVICE_STATE_Default;
            }
            break;
        case REQ_GetDescriptor:
            if ((bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE)) ||
                (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_INTERFACE)))
            {
                const void *descPtr;
                uint16_t descSize;

                if (_ctrlReq.wValue == (DTYPE_String << 8 | UZE_INTERNAL_SERIAL))
                {
                    SigDesc sigDesc;
                    sigDesc.type = DTYPE_String;
                    sigDesc.size = USB_STRING_LEN(INTERNAL_ZERIAL_LENGTH_BITS / 4);
                    Device_GetSerialString(sigDesc.unicodeString);
                    *p_ueintx &= ~(1<<rxstpi);
                    write_Control_Stream_LE(&sigDesc, sizeof(sigDesc));
                    *p_ueintx &= ~(1<<rxouti | 1<<fifocon);
                    return;
                }

                if ((descSize = getDescriptor(_ctrlReq.wValue, _ctrlReq.wIndex, &descPtr)) == 0)
                    return;

                UEINTX &= ~(1<<RXSTPI);     // clear setup
                write_Control_PStream_LE(descPtr, descSize);
                UEINTX &= ~(1<<RXOUTI | 1<<FIFOCON);    // clear out
            }

            break;
        case REQ_GetConfiguration:
            if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE))
            {
                UEINTX &= ~(1<<RXSTPI);     // clear setup
                write8(USB_Device_ConfigurationNumber);
                UEINTX &= ~(1<<TXINI | 1<<FIFOCON); // clear in
                clearStatusStage();
            }
            break;
        case REQ_SetConfiguration:
            if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
            {
                if ((uint8_t)_ctrlReq.wValue > FIXED_NUM_CONFIGURATIONS)
                    return;

                *p_ueintx &= ~(1<<rxstpi);
                USB_Device_ConfigurationNumber = (uint8_t)_ctrlReq.wValue;
                clearStatusStage();

                if (USB_Device_ConfigurationNumber)
                    state = DEVICE_STATE_Configured;
                else
                    state = *p_udaddr & 1<<adden ? DEVICE_STATE_Configured : DEVICE_STATE_Powered;

                configureEndpoint(_inpoint);
                configureEndpoint(_outpoint);
                *p_udien |= 1<<sofe;
            }
            break;
        default:
            break;
        }
    }

    if (*p_ueintx & 1<<rxstpi)      // setup received?
    {
        *p_ueintx &= ~(1<<rxstpi);  // clear setup
        *p_ueconx |= 1<<stallrq;    // stall transaction
    }
}

USBSD::USBSD() :
    _sd(&_board.pin9),
    _inpoint(MASS_STORAGE_IN_EPADDR, MASS_STORAGE_IO_EPSIZE, EP_TYPE_BULK, 1),
    _outpoint(MASS_STORAGE_OUT_EPADDR, MASS_STORAGE_IO_EPSIZE, EP_TYPE_BULK, 1)
{
    _sd.init(SPI_FULL_SPEED);
    _blocks = _sd.cardSize();
    *p_usbcon &= ~(1<<otgpade);
    *p_uhwcon |= 1<<UVREGE;
    PLLFRQ = (1 << PDIV2);
    *p_usbcon &= ~(1<<vbuste);
    *p_udien = 0;
    *p_usbint = 0;
    *p_udint = 0;
    *p_usbcon &= ~(1<<usbe);
    *p_usbcon |= 1<<usbe;
    *p_usbcon &= ~(1<<frzclk);
    PLLCSR = 0; // pll off
    state = DEVICE_STATE_Unattached;
    USB_Device_ConfigurationNumber  = 0;
    USB_Device_RemoteWakeupEnabled  = false;
    USB_Device_CurrentlySelfPowered = false;
    UDCON &= ~(1<<LSM); // full speed
    USBCON |= 1<<VBUSTE;
    configureEndpoint(0, 0, 8, 1);
    UDINT &= ~(1<<SUSPI);
    UDIEN |= 1<<SUSPE;
    UDIEN |= 1<<EORSTE;
    UDCON &= ~(1<<DETACH);  // attach
    USBCON |= 1<<OTGPADE;
}





