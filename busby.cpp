/*
Adapted from LUFA code by Dean Camera
*/

#include "busby.h"
#include "schip.h"
#include "pgmspees.h"
#include "int.h"

#define NULL 0

USB *USB::instance;

uint8_t USB::getEndpointDirection() const
{
    return *p_uecfg0x & 1<<EPDIR ? ENDPOINT_DIR_IN : ENDPOINT_DIR_OUT;
}

uint32_t USB::read32() const
{
    union { uint32_t value; uint8_t bytes[4]; } Data;
    Data.bytes[0] = *p_uedatx;
    Data.bytes[1] = *p_uedatx;
    Data.bytes[2] = *p_uedatx;
    Data.bytes[3] = *p_uedatx;
    return Data.value;
}

void USB::write32(uint32_t dat) const
{
    *p_uedatx = dat &  0xFF;
    *p_uedatx = dat >> 8;
    *p_uedatx = dat >> 16;
    *p_uedatx = dat >> 24;
}

void USB::write32be(uint32_t dat) const
{
    *p_uedatx = dat >> 24;
    *p_uedatx = dat >> 16;
    *p_uedatx = dat >> 8;
    *p_uedatx = dat & 0xff;
}

void USB::Device_GetSerialString(uint16_t *unicodeString)
{
    uint8_t currentGlobalInt = SREG;
    zli();
    uint8_t sigReadAddress = INTERNAL_SERIAL_START_ADDRESS;

    for (uint8_t serCharNum = 0; serCharNum < INTERNAL_SERIAL_LENGTH_BITS / 4; serCharNum++)
    {
        uint8_t serByte = boot_signature_byte_get(sigReadAddress);

        if (serCharNum & 0x01)
        {
            serByte >>= 4;
            sigReadAddress++;
        }

        serByte &= 0x0F;
        unicodeString[serCharNum] = serByte >= 10 ? ('A' - 10) + serByte : '0' + serByte;
    }

    GCC_MEMORY_BARRIER();
    SREG = currentGlobalInt;
    GCC_MEMORY_BARRIER();
}

uint8_t USB::waitUntilReady() const
{
    uint16_t TimeoutMSRem = USB_STREAM_TIMEOUT_MS;
    uint16_t previousFrameNumber = *p_udfnum;

    while (true)
    {
        if (getEndpointDirection() == ENDPOINT_DIR_IN)
        {
            if (*p_ueintx & 1<<txini)
                return ENDPOINT_READYWAIT_NoError;
        }
        else
        {
            if (*p_ueintx & 1<<rxouti)
                return ENDPOINT_READYWAIT_NoError;
        }

        uint8_t USB_DeviceState_LCL = state;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_READYWAIT_DeviceDisconnected;

        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_READYWAIT_BusSuspended;

        if (*p_ueconx & 1<<stallrq)
            return ENDPOINT_READYWAIT_EndpointStalled;

        uint16_t currentFrameNumber = *p_udfnum;

        if (currentFrameNumber != previousFrameNumber)
        {
            previousFrameNumber = currentFrameNumber;

            if (!(TimeoutMSRem--))
                return ENDPOINT_READYWAIT_Timeout;
        }
    }
}

void USB::clearStatusStage() const
{
    if (_ctrlReq.bmRequestType & REQDIR_DEVICETOHOST)
    {
        while ((*p_ueintx & 1<<rxouti) == 0)
            if (state == DEVICE_STATE_UNATTACHED)
                return;

        *p_ueintx &= ~(1<<rxouti | 1<<fifocon); // clear out
    }
    else
    {
        while ((*p_ueintx & 1<<txini) == 0)
            if (state == DEVICE_STATE_UNATTACHED)
                return;

        *p_ueintx &= ~(1<<txini | 1<<fifocon); // endpoint clear in
    }
}

bool USB::configureEndpoint(uint8_t addr, uint8_t Type, uint16_t Size, uint8_t banks)
{
    uint8_t Number = addr & ENDPOINT_EPNUM_MASK;

    if (Number >= ENDPOINT_TOTAL_ENDPOINTS)
        return false;

    uint8_t cfg0 = Type << EPTYPE0 | (addr & ENDPOINT_DIR_IN ? 1<<EPDIR : 0);
    uint8_t cfg1 = 1<<ALLOC | (banks > 1 ? 1<<EPBK0 : 0) | Endpoint_BytesToEPSizeMask(Size);

    for (uint8_t epNum = Number; epNum < ENDPOINT_TOTAL_ENDPOINTS; epNum++)
    {
        uint8_t UECFG0XTemp, UECFG1XTemp, UEIENXTemp;
        selectEndpoint(epNum);

        if (epNum == Number)
        {
            UECFG0XTemp = cfg0;
            UECFG1XTemp = cfg1;
            UEIENXTemp  = 0;
        }
        else
        {
            UECFG0XTemp = *p_uecfg0x;
            UECFG1XTemp = *p_uecfg1x;
            UEIENXTemp  = *p_ueienx;
        }

        if ((UECFG1XTemp & 1<<alloc) == 0)
            continue;

        *p_ueconx &= ~(1<<epen);
        *p_uecfg1x &= ~(1<<alloc);
        *p_ueconx |= 1<<epen;
        *p_uecfg0x = UECFG0XTemp;
        *p_uecfg1x = UECFG1XTemp;
        *p_ueienx = UEIENXTemp;

        if ((UESTA0X & 1<<CFGOK) == 0)
            return false;
    }

    selectEndpoint(Number);
    return true;
}

USB::USB() : _control(ENDPOINT_CONTROLEP, 8, EP_TYPE_CONTROL, 1)
{
    instance = this;
}

void USB::Device_ClearSetFeature()
{
    switch (_ctrlReq.bmRequestType & CONTROL_REQTYPE_RECIPIENT)
    {
    case REQREC_DEVICE:
        if ((uint8_t)_ctrlReq.wValue == FEATURE_SEL_DeviceRemoteWakeup)
            _remoteWakeupEn = _ctrlReq.bRequest == REQ_SetFeature;
        else
            return;

        break;
    case REQREC_ENDPOINT:
        if ((uint8_t)_ctrlReq.wValue == FEATURE_SEL_ENDPOINTHALT)
        {
            uint8_t EndpointIndex = ((uint8_t)_ctrlReq.wIndex & ENDPOINT_EPNUM_MASK);

            if (EndpointIndex == ENDPOINT_CONTROLEP)
                return;

            selectEndpoint(EndpointIndex);

            if (*p_ueconx & 1<<epen)
            {
                if (_ctrlReq.bRequest == REQ_SetFeature)
                {
                    *p_ueconx |= 1<<stallrq;
                }
                else
                {
                    *p_ueconx |= 1<<stallrqc;
                    *p_uerst = 1<<(EndpointIndex & ENDPOINT_EPNUM_MASK);
                    *p_uerst = 0;
                    *p_ueconx |= 1<<rstdt;
                }
            }
        }

        break;
    default:
        return;
    }

    selectEndpoint(ENDPOINT_CONTROLEP);
    *p_ueintx &= ~(1<<rxstpi);
    clearStatusStage();
}

uint8_t USB::write_Control_Stream_LE(const void* const buffer, uint16_t len)
{
    uint8_t *dataStream = (uint8_t*)buffer;
    bool lastPacketFull = false;

    if (len > _ctrlReq.wLength)
        len = _ctrlReq.wLength;
    else if (!len)
        *p_ueintx &= ~(1<<txini | 1<<fifocon); // endpoint clear in

    while (len || lastPacketFull)
    {
        uint8_t usbDeviceState_LCL = state;

        if (usbDeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;

        if (usbDeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;

        if (*p_ueintx & 1<<rxstpi)  // setup received?
            return ENDPOINT_RWCSTREAM_HostAborted;

        if (*p_ueintx & 1<<rxouti)  // out received?
            break;

        if (*p_ueintx & 1<<txini)   // in ready?
        {
            uint16_t bytesInEp = bytesInEndpoint();

            while (len && bytesInEp < _control.size)
            {
                write8(*dataStream);
                dataStream++;
                len--;
                bytesInEp++;
            }

            lastPacketFull = bytesInEp == _control.size;
            *p_ueintx &= ~(1<<txini | 1<<fifocon); // endpoint clear in
        }
    }

    while ((*p_ueintx & 1<<rxouti) == 0)
    {
        uint8_t usbDeviceState_LCL = state;

        if (usbDeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;

        if (usbDeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

uint8_t USB::write_Control_PStream_LE(const void* const Buffer, uint16_t length)
{
    uint8_t *DataStream = (uint8_t*)Buffer;
    bool LastPacketFull = false;

    if (length > _ctrlReq.wLength)
        length = _ctrlReq.wLength;
    else if (!length)
        *p_ueintx &= ~(1<<txini | 1<<fifocon); // endpoint clear in

    while (length || LastPacketFull)
    {
        uint8_t USB_DeviceState_LCL = state;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;

        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;

        if (*p_ueintx & 1<<rxstpi)  // setup received?
            return ENDPOINT_RWCSTREAM_HostAborted;

        if (*p_ueintx & 1<<rxouti)
            break;

        if (*p_ueintx & 1<<txini)
        {
            uint16_t bytesInEp = bytesInEndpoint();

            while (length && bytesInEp < _control.size)
            {
                write8(pgm_read_byte(DataStream));
                DataStream++;
                length--;
                bytesInEp++;
            }

            LastPacketFull = bytesInEp == _control.size;
            *p_ueintx &= ~(1<<txini | 1<<fifocon); // endpoint clear in
        }
    }

    while ((*p_ueintx & 1<<rxouti) == 0)
    {
        uint8_t USB_DeviceState_LCL = state;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;

        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

uint8_t USB::Endpoint_BytesToEPSizeMask(const uint16_t bytes) const
{
    uint8_t maskVal = 0;
    uint16_t checkBytes = 8;

    while (checkBytes < bytes)
    {
        maskVal++;
        checkBytes <<= 1;
    }

    return maskVal<<EPSIZE0;
}

bool USB::configureEndpoint(Endpoint &ep)
{
    return configureEndpoint(ep.addr, ep.type, ep.size, ep.banks);
}

uint8_t USB::nullStream(uint16_t len, uint16_t * const bytesProcessed)
{
    uint8_t errCode = waitUntilReady();
    uint16_t bytesInTransfer = 0;
    
    if (errCode)
        return errCode;

    if (bytesProcessed != NULL)
        len -= *bytesProcessed;

    while (len)
    {
        if ((*p_ueintx & 1<<rwal) == 0) // read-write not allowed?
        {
            *p_ueintx &= ~(1<<txini | 1<<fifocon);  // clear in
            
            if (bytesProcessed != NULL)
            {
                *bytesProcessed += bytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            errCode = waitUntilReady();

            if (errCode)
                return errCode;
        }
        else
        {
            write8(0);
            len--;
            bytesInTransfer++;
        }
    }
    
    return ENDPOINT_RWSTREAM_NoError;
}

uint8_t USB::readStream(void * const buf, uint16_t len, uint16_t * const bytes)
{
    uint8_t *dataStream = (uint8_t *)buf;
    uint16_t bytesInTransfer = 0;
    uint8_t errorCode = waitUntilReady();

    if (errorCode)
        return errorCode;

    if (bytes != NULL)
    {
        len -= *bytes;
        dataStream += *bytes;
    }

    while (len)
    {
        if ((*p_ueintx & 1<<rwal) == 0)    // read-write allowed?
        {
            *p_ueintx &= ~(1<<rxouti | 1<<fifocon);    // clear out

            if (bytes != NULL)
            {
                *bytes += bytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }
            
            errorCode = waitUntilReady();

            if (errorCode)
                return errorCode;
        }
        else
        {
            *dataStream = read8();
            dataStream += 1;
            len--;
            bytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;

}

void USB::com()
{
    uint8_t prevSelectedEndp = getCurrentEndpoint();
    _control.select();
    *p_ueienx &= ~(1<<rxstpe);
    zei();
    procCtrlReq();
    _control.select();
    *p_ueienx |= 1<<rxstpe;
    selectEndpoint(prevSelectedEndp);
}

#if 0
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

void USB::gen()
{   
    if (*p_udint & 1<<sofi && *p_udien & 1<<sofe)
        *p_udint &= ~(1<<sofi);

    if (*p_usbint & 1<<vbusti && *p_usbcon & 1<<vbuste)
    {
        *p_usbint &= ~(1<<vbusti);

        if (*p_usbsta & 1<<vbus)
        {
            *p_pllcsr = USB_PLL_PSC;
            *p_pllcsr = USB_PLL_PSC | 1<<plle;

            while ((*p_pllcsr & 1<<plock) == 0)
                ;

            state = DEVICE_STATE_POWERED;
            connect();
        }
        else
        {
            *p_pllcsr = 0;
            state = DEVICE_STATE_UNATTACHED;
        }
    }

    if (*p_udint & 1<<suspi && *p_udien & 1<<suspe)
    {
        *p_udien &= ~(1<<suspe);   // disable int suspe
        *p_udien |= 1<<wakeupe;    // enable int wakeup
        *p_usbcon |= 1<<frzclk;
        *p_pllcsr = 0;
        state = DEVICE_STATE_SUSPENDED;
    }

    if (*p_udint & 1<<wakeupi && *p_udien & 1<<wakeupe)
    {
        *p_pllcsr = USB_PLL_PSC;
        *p_pllcsr = USB_PLL_PSC | 1<<plle;   // PLL on
        while ((*p_pllcsr & 1<<plock) == 0);   // PLL is ready?
        *p_usbcon &= ~(1<<frzclk);
        *p_udint &= ~(1<<wakeupi);
        *p_udien &= ~(1<<wakeupe);
        *p_udien |= 1<<suspe;

        if (confNum)
            state = DEVICE_STATE_Configured;
        else
            state = *p_udaddr & 1<<adden ? DEVICE_STATE_Configured : DEVICE_STATE_POWERED;
    }

    if (*p_udint & 1<<eorsti && *p_udien & 1<<eorste)
    {
        *p_udint &= ~(1<<eorsti);      // clear INT EORSTI
        state = DEVICE_STATE_Default;
        confNum = 0;
        *p_udint &= ~(1<<suspi);       // clear INT SUSPI
        *p_udien &= ~(1<<suspe);       // disable INT SUSPE
        *p_udien |= 1<<wakeupe;
        configureEndpoint(_control);
        *p_ueienx |= 1<<rxstpe;
    }
}

uint8_t USB::readControlStreamLE(void * const buf, uint16_t len)
{
    uint8_t *dataStream = (uint8_t *)buf;
    
    if (!len)
        *p_ueintx &= ~(1<<rxouti | 1<<fifocon); // clear out

    while (len)
    {
        uint8_t usbDeviceState_LCL = state;

        if (usbDeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;

        if (usbDeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;

        if (*p_ueintx & 1<<rxstpi)  // setup received?
            return ENDPOINT_RWCSTREAM_HostAborted;
        
        if (*p_ueintx & 1<<rxouti)   // is out received?
        {
            while (len && bytesInEndpoint())
            {
                *dataStream = read8();
                dataStream++;
                len--;
            }

            *p_ueintx &= ~(1<<rxouti | 1<<fifocon); // clear out
        }
    }

    while ((*p_ueintx & 1<<txini) == 0)    // in ready?
    {
        uint8_t usbDeviceState_LCL = state;

        if (usbDeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;

        if (usbDeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

void USB::procCtrlReq()
{
    uint8_t *RequestHeader = (uint8_t*)&_ctrlReq;

    for (uint8_t i = 0; i < sizeof(USBRequest); i++)
        RequestHeader[i] = read8();

    customCtrl();

    if (*p_ueintx & 1<<rxstpi)  // setup received?
    {
        const uint8_t bmRequestType = _ctrlReq.bmRequestType;

        switch (_ctrlReq.bRequest)
        {
        case REQ_GETSTATUS:
            if ((bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE)) ||
                (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_ENDPOINT)))
            {
                uint8_t currentStatus = 0;

                switch (bmRequestType)
                {
                case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE):
                    if (USB_Device_CurrentlySelfPowered)
                        currentStatus |= FEATURE_SELFPOWERED_ENABLED;

                    if (_remoteWakeupEn)
                        currentStatus |= FEATURE_REMOTE_WAKEUP_ENABLED;
  
                    break;
                case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_ENDPOINT):
                    selectEndpoint((uint8_t)_ctrlReq.wIndex & ENDPOINT_EPNUM_MASK);
                    currentStatus = *p_ueconx & 1<<stallrq;
                    _control.select();
                    break;
                default:
                    return;
                }

                *p_ueintx &= ~(1<<rxstpi); // clear setup
                write16le(currentStatus);
                *p_ueintx &= ~(1<<txini | 1<<fifocon); // clear in
                clearStatusStage();
            }

            break;
        case REQ_CLEARFEATURE:
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
                uint8_t deviceAddress = _ctrlReq.wValue & 0x7F;
                setDevAddr(deviceAddress);
                *p_ueintx &= ~(1<<rxstpi); // clear setup
                clearStatusStage();

                while ((*p_ueintx & 1<<txini) == 0) // in ready?
                    ;

                *p_udaddr |= 1<<adden; // enable dev addr
                state = deviceAddress ? DEVICE_STATE_ADDRESSED : DEVICE_STATE_Default;
            }
            break;
        case REQ_GetDescriptor:
            if ((bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE)) ||
                (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_INTERFACE)))
            {
                const void *descPtr;
                uint16_t descSize;

                if (_ctrlReq.wValue == (DTYPE_STRING << 8 | USE_INTERNAL_SERIAL))
                {
                    SigDesc sigDesc;
                    sigDesc.type = DTYPE_STRING;
                    sigDesc.size = USB_STRING_LEN(INTERNAL_SERIAL_LENGTH_BITS / 4);
                    Device_GetSerialString(sigDesc.unicodeString);
                    *p_ueintx &= ~(1<<rxstpi);
                    write_Control_Stream_LE(&sigDesc, sizeof(sigDesc));
                    *p_ueintx &= ~(1<<rxouti | 1<<fifocon); // clear out
                    return;
                }

                if ((descSize = getDesc(_ctrlReq.wValue, _ctrlReq.wIndex, &descPtr)) == 0)
                    return;

                *p_ueintx &= ~(1<<rxstpi);     // clear setup
                write_Control_PStream_LE(descPtr, descSize);
                *p_ueintx &= ~(1<<rxouti | 1<<fifocon);    // clear out
            }

            break;
        case REQ_GetConfiguration:
            if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE))
            {
                *p_ueintx &= ~(1<<rxstpi);     // clear setup
                write8(confNum);
                *p_ueintx &= ~(1<<txini | 1<<fifocon); // clear in
                clearStatusStage();
            }
            break;
        case REQ_SETCONFIGURATION:
            if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
            {
                if ((uint8_t)_ctrlReq.wValue > FIXED_NUM_CONFIGURATIONS)
                    return;

                *p_ueintx &= ~(1<<rxstpi);
                confNum = (uint8_t)_ctrlReq.wValue;
                clearStatusStage();

                if (confNum)
                    state = DEVICE_STATE_Configured;
                else
                    state = *p_udaddr & 1<<adden ? DEVICE_STATE_Configured : DEVICE_STATE_POWERED;

                configure();
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

uint8_t USB::writeStream2(const void * const buf, uint16_t len, uint16_t * const bytes)
{
    uint8_t *dataStream = (uint8_t *)buf;
    uint16_t bytesInTransfer = 0;
    uint8_t errorCode = waitUntilReady();

    if (errorCode)
        return errorCode;

    if (bytes != NULL)
    {
        len -= *bytes;
        dataStream += *bytes;
    }

    while (len)
    {
        if ((*p_ueintx & 1<<rwal) == 0)    // read-write allowed?
        {
            *p_ueintx &= ~(1<<txini | 1<<fifocon); // endpoint clear in
            
            if (bytes != NULL)
            {
                *bytes += bytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            errorCode = waitUntilReady();

            if (errorCode)
                return errorCode;
        }
        else
        {
            write8(*dataStream);
            dataStream += 1;
            len--;
            bytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}




