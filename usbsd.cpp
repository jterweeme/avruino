/*
Adapted from LUFA code by Dean Camera
*/

#include "usbsd.h"
#include <avr/pgmspace.h>
#include <ctype.h>

static constexpr uint8_t
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
    MS_CZCP_SCSITRANSPARENTSUBCLASS = 0x06,
    MS_CZCP_MassStorageClass = 0x08,
    MS_CZCP_BulkOnlyTransportProtocol = 0x50,
    MASS_STORAGE_IO_EPSIZE = 64,
    M2S_REQ_GetMaxLUN = 0xfe,
    M2S_REQ_MassStorageReset = 0xff,
    MASS_STORAGE_IN_EPADDR = ENDPOINT_DIR_IN | 3,
    MASS_STORAGE_OUT_EPADDR = ENDPOINT_DIR_OUT | 4,
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
    0x2045,
    0x0001,
    STRING_ID_MANUFACTURER,
    STRING_ID_PRODUCT,
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
        DTYPE_CONFIGURATION,
        sizeof(MyConf),
        1,
        1,
        0,    // no descriptor
        USB_CONFIG_ATTR_RESERVED,
        USB_CONFIG_POWER_MA(100)
    },
    {
        sizeof(DescIface),
        DTYPE_INTERFACE,
        0, // mass storage
        0,
        2,
        MS_CZCP_MassStorageClass,
        MS_CZCP_SCSITRANSPARENTSUBCLASS,
        MS_CZCP_BulkOnlyTransportProtocol,
        0
    },
    {
        sizeof(DescEndpoint),
        DTYPE_ENDPOINT,
        MASS_STORAGE_IN_EPADDR,
        EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        MASS_STORAGE_IO_EPSIZE,
        0x05
    },
    {
        sizeof(DescEndpoint),
        DTYPE_ENDPOINT,
        MASS_STORAGE_OUT_EPADDR,
        EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA,
        MASS_STORAGE_IO_EPSIZE,
        0x05
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

static const DescString<23> PROGMEM productString =
{
    USB_STRING_LEN(22),
    DTYPE_STRING,
    L"LUFA Mass Storage Demo"
};

uint16_t USBSD::getDesc(uint16_t wValue, uint16_t wIndex, const void **const descAddr)
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

    switch (cmdBlock.cmdData[0])
    {
    case ZCZI_CMD_INQUIRY:
    {
        uint16_t allocLen = cmdBlock.cmdData[3] << 8 | cmdBlock.cmdData[4];
        uint16_t bytesTransferred = min(allocLen, sizeof(InquiryData));

        if ((cmdBlock.cmdData[1] & (1<<0 | 1<<1)) || cmdBlock.cmdData[2])
        {
            senseData.vanalles = ZCZI_SENSE_KEY_ILLEGAL_REQUEST << 4;
            senseData.AdditionalSenseCode = ZCZI_ASENSE_INVALID_FIELD_IN_CDB;
            senseData.AdditionalSenseQualifier = ZCZI_ASENSEQ_NO_QUALIFIER;
            break;
        }

        writeStream2(&InquiryData, bytesTransferred, NULL);
        nullStream(allocLen - bytesTransferred, NULL);
        *p_ueintx &= ~(1<<txini | 1<<fifocon); // clear in
        cmdBlock.DataTransferLength -= bytesTransferred;
        success = true;
    }
        break;
    case ZCZI_CMD_REQUEST_SENSE:
    {
        uint8_t AllocationLength = cmdBlock.cmdData[4];
        uint8_t BytesTransferred = min((size_t)AllocationLength, sizeof(senseData));
        writeStream2(&senseData, BytesTransferred, NULL);
        nullStream(AllocationLength - BytesTransferred, NULL);
        *p_ueintx &= ~(1<<txini | 1<<fifocon); // clear in
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
        if ((cmdBlock.cmdData[1] & 1<<2) == 0)
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
        *p_ueintx &= ~(1<<txini | 1<<fifocon); // clear in
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
    uint32_t blockAddr = (uint32_t)cmdBlock.cmdData[2] << 24 |
                (uint32_t)cmdBlock.cmdData[3] << 16 |
                (uint32_t)cmdBlock.cmdData[4] << 8 | (uint32_t)cmdBlock.cmdData[5];

    uint32_t totalBlocks = cmdBlock.cmdData[7] << 8 | cmdBlock.cmdData[8];

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
        if ((*p_ueintx & 1<<rwal) == 0)     // r/w not allowed?
        {
            *p_ueintx &= ~(1<<rxouti | 1<<fifocon); // clear out
            
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

    if ((*p_ueintx & 1<<rwal) == 0)    // r/w not allowed?
        *p_ueintx &= ~(1<<rxouti | 1<<fifocon);    // clear out
}

inline void USBSD::_sdReadBlock(const uint32_t block)
{
    uint8_t buf[512];
    _sd.readBlock(block, buf);

    for (uint16_t byte = 0; byte < BLOCKSIZE; )
    {
        if ((*p_ueintx & 1<<rwal) == 0)    // r/w not allowed?
        {
            *p_ueintx &= ~(1<<txini | 1<<fifocon); // clear in

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

    if ((*p_ueintx & 1<<rwal) == 0)            // buffer full?
        *p_ueintx &= ~(1<<txini | 1<<fifocon); // then clear it
}

void USBSD::MassStorage_Task()
{
	if (state != DEVICE_STATE_Configured)
	    return;

    if (ReadInCommandBlock())
    {
        if (cmdBlock.flags & MS_KOMMAND_DIR_DATA_IN)
            selectEndpoint(MASS_STORAGE_IN_EPADDR);

		cmdStatus.Status = decodeSCSICmd() ? MS_ZCZI_COMMAND_Pass : MS_ZCZI_COMMAND_Fail;
		cmdStatus.Tag = cmdBlock.tag;
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
        *p_ueconx |= 1<<STALLRQC;  // clear stall
        *p_ueconx |= 1<<RSTDT; // reset data toggle
		selectEndpoint(MASS_STORAGE_IN_EPADDR);
        *p_ueconx |= 1<<STALLRQC;  // clear stall
        *p_ueconx |= 1<<RSTDT; // reset data toggle
		IsMassStoreReset = false;
	}
}

bool USBSD::ReadInCommandBlock()
{
	selectEndpoint(MASS_STORAGE_OUT_EPADDR);

	if ((*p_ueintx & 1<<rxouti) == 0)  // is out received?
	    return false;

	uint16_t BytesTransferred = 0;

	while (readStream(&cmdBlock, sizeof(cmdBlock) - sizeof(cmdBlock.cmdData),
	    &BytesTransferred) == ENDPOINT_RWSTREAM_IncompleteTransfer)
	{
		if (IsMassStoreReset)
		    return false;
	}

	if (cmdBlock.signature != M2S_CBW_SIGNATURE || cmdBlock.LUN >= TOTAL_LUNS ||
        cmdBlock.flags & 0x1F || cmdBlock.SCSICommandLength == 0 ||
		(cmdBlock.SCSICommandLength > sizeof(cmdBlock.cmdData)))
	{
        *p_ueconx |= 1<<stallrq;        // stall transaction
		selectEndpoint(MASS_STORAGE_IN_EPADDR);
        *p_ueconx |= 1<<stallrq;        // stall transaction
		return false;
	}

	BytesTransferred = 0;

	while (readStream(&cmdBlock.cmdData, cmdBlock.SCSICommandLength,
	                               &BytesTransferred) == ENDPOINT_RWSTREAM_IncompleteTransfer)
	{
		if (IsMassStoreReset)
		    return false;
	}

    *p_ueintx &= ~(1<<rxouti | 1<<fifocon);    // clear out
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

    *p_ueintx &= ~(1<<txini | 1<<fifocon); // clear in
}

void USBSD::customCtrl()
{
    if ((*p_ueintx & 1<<rxstpi) == 0)
        return;

    if (_ctrlReq.wIndex != _control.addr)
        return;
    
    switch (_ctrlReq.bRequest)
    {
    case M2S_REQ_MassStorageReset:
        if (_ctrlReq.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            *p_ueintx &= ~(1<<rxstpi); // clear setup
            clearStatusStage();
            IsMassStoreReset = true;
        }

        break;
    case M2S_REQ_GetMaxLUN:
        if (_ctrlReq.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            *p_ueintx &= ~(1<<rxstpi); // clear setup
            write8(TOTAL_LUNS - 1);
            *p_ueintx &= ~(1<<txini | 1<<fifocon); // clear in
            clearStatusStage();
        }
        break;
    }
}

void USBSD::configure()
{
    configureEndpoint(_inpoint);
    configureEndpoint(_outpoint);
    *p_udien |= 1<<sofe;
}

USBSD::USBSD() :
    _sd(&_board.pin9),
    _inpoint(MASS_STORAGE_IN_EPADDR, MASS_STORAGE_IO_EPSIZE, EP_TYPE_BULK, 1),
    _outpoint(MASS_STORAGE_OUT_EPADDR, MASS_STORAGE_IO_EPSIZE, EP_TYPE_BULK, 1)
{
    _sd.init(SPI_FULL_SPEED);
    _blocks = _sd.cardSize();
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





