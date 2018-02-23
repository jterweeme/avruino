#include "bogota.h"
#include <avr/pgmspace.h>

#if 0
void Busby::USB_DeviceTask()
{
    if (state == DEVICE_STATE_UNATTACHED)
        return;

    uint8_t PrevEndpoint = Endpoint_GetCurrentEndpoint();
    Endpoint_SelectEndpoint(0);

    if (UEINTX & 1<<RXSTPI)
        procCtrlReq();

    Endpoint_SelectEndpoint(PrevEndpoint);
}
#endif

Busby::Busby()
{
    instance = this;
}

uint16_t Busby::Endpoint_BytesInEndpoint()
{
    return (((uint16_t)UEBCHX << 8) | UEBCLX);
}

uint8_t Busby::Endpoint_Write_PStream_LE(const void * const Buffer,
                            uint16_t Length, uint16_t* const BytesProcessed)
{
    uint8_t *DataStream = (uint8_t*)Buffer;
    uint16_t BytesInTransfer = 0;
    uint8_t ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        DataStream += *BytesProcessed;
    }

    while (Length)
    {
        if (!(Endpoint_IsReadWriteAllowed()))
        {
            Endpoint_ClearIN();

            if (BytesProcessed != NULL)
            {
                *BytesProcessed += BytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            if ((ErrorCode = Endpoint_WaitUntilReady()))
                return ErrorCode;
        }
        else 
        {
            Endpoint_Write_8(pgm_read_byte(DataStream));
            DataStream += 1;
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

void Busby::com()
{
    uint8_t PrevSelectedEndpoint = Endpoint_GetCurrentEndpoint();
    Endpoint_SelectEndpoint(0);
    *p_ueienx &= ~(1<<rxstpe);
    sei();
    procCtrlReq();
    Endpoint_SelectEndpoint(0);
    *p_ueienx |= 1<<rxstpe;
    Endpoint_SelectEndpoint(PrevSelectedEndpoint);
}

uint8_t Busby::Endpoint_WaitUntilReady()
{
    uint8_t TimeoutMSRem = USB_STREAM_TIMEOUT_MS;
    uint16_t PreviousFrameNumber = USB_Device_GetFrameNumber();

    for (;;)
    {
        if (Endpoint_GetEndpointDirection() == ENDPOINT_DIR_IN)
        {
            if (UEINTX & 1<<TXINI)
                return ENDPOINT_READYWAIT_NoError;
        }
        else
        {
            if (UEINTX & 1<<RXOUTI)
                return ENDPOINT_READYWAIT_NoError;
        }

        uint8_t USB_DeviceState_LCL = state;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_READYWAIT_DeviceDisconnected;

        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_READYWAIT_BusSuspended;

        if (UECONX & 1<<STALLRQ)
            return ENDPOINT_READYWAIT_EndpointStalled;

        uint16_t CurrentFrameNumber = USB_Device_GetFrameNumber();

        if (CurrentFrameNumber != PreviousFrameNumber)
        {
            PreviousFrameNumber = CurrentFrameNumber;

            if (!(TimeoutMSRem--))
                return ENDPOINT_READYWAIT_Timeout;
        }
    }
}

void Busby::USB_Device_GetConfiguration()
{
    UEINTX &= ~(1<<RXSTPI);
    Endpoint_Write_8(USB_Device_ConfigurationNumber);
    Endpoint_ClearIN();
    Endpoint_ClearStatusStage();
}

void Busby::USB_Device_GetInternalSerialDescriptor()
{
    struct
    {
        USB_Descriptor_Header_t Header;
        uint16_t                UnicodeString[INTERNAL_SERIAL_LENGTH_BITS / 4];
    } SignatureDescriptor;

    SignatureDescriptor.Header.Type = DTYPE_STRING;
    SignatureDescriptor.Header.Size = USB_STRING_LEN(INTERNAL_SERIAL_LENGTH_BITS / 4);
    USB_Device_GetSerialString(SignatureDescriptor.UnicodeString);
    UEINTX &= ~(1<<RXSTPI);
    write_Control_Stream_LE(&SignatureDescriptor, sizeof(SignatureDescriptor));
    Endpoint_ClearOUT();
}

uint8_t Busby::writeStream2(const void * const Buffer, uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream = (uint8_t*)Buffer;
    uint16_t BytesInTransfer = 0;
    uint8_t  ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        DataStream += *BytesProcessed;
    }

    while (Length)
    {
        if (Endpoint_IsReadWriteAllowed() == 0)
        {
            Endpoint_ClearIN();

            if (BytesProcessed != NULL)
            {
                *BytesProcessed += BytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            if ((ErrorCode = Endpoint_WaitUntilReady()))
              return ErrorCode;
        }
        else
        {
            Endpoint_Write_8(*DataStream);
            DataStream += 1;
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

uint8_t Busby::readStream(void * const Buffer, uint16_t Length,
                            uint16_t* const BytesProcessed)
{
    uint8_t* DataStream      = (uint8_t*)Buffer;
    uint16_t BytesInTransfer = 0;
    uint8_t  ErrorCode;

    if ((ErrorCode = Endpoint_WaitUntilReady()))
      return ErrorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        DataStream += *BytesProcessed;
    }

    while (Length)
    {
        if ((Endpoint_IsReadWriteAllowed()) == 0)
        {
            Endpoint_ClearOUT();

            if (BytesProcessed != NULL)
            {
                *BytesProcessed += BytesInTransfer;
                return ENDPOINT_RWSTREAM_IncompleteTransfer;
            }

            if ((ErrorCode = Endpoint_WaitUntilReady()))
              return ErrorCode;
        }
        else
        {
            *DataStream = Endpoint_Read_8();
            DataStream += 1;
            Length--;
            BytesInTransfer++;
        }
    }

    return ENDPOINT_RWSTREAM_NoError;
}

uint8_t Busby::write_Control_Stream_LE(const void* const Buffer, uint16_t Length)
{
    uint8_t *DataStream = (uint8_t*)Buffer;
    bool LastPacketFull = false;

    if (Length > _ctrlReq.wLength)
        Length = _ctrlReq.wLength;
    else if (Length == 0)
        Endpoint_ClearIN();

    while (Length || LastPacketFull)
    {
        uint8_t USB_DeviceState_LCL = state;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;
        if (Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;
        if (Endpoint_IsOUTReceived())
            break;

        if (UEINTX & 1<<TXINI)
        {
            uint16_t BytesInEndpoint = Endpoint_BytesInEndpoint();

            while (Length && (BytesInEndpoint < USB_Device_ControlEndpointSize))
            {
                Endpoint_Write_8(*DataStream);
                DataStream += 1;
                Length--;
                BytesInEndpoint++;
            }

            LastPacketFull = (BytesInEndpoint == USB_Device_ControlEndpointSize);
            Endpoint_ClearIN();
        }
    }

    while ((Endpoint_IsOUTReceived()) == 0)
    {
        uint8_t USB_DeviceState_LCL = state;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;

        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;

        if (Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

uint8_t Busby::readControlStreamLE(void* const Buffer, uint16_t Length)
{
    uint8_t* DataStream = (uint8_t*)Buffer;

    if (!(Length))
      Endpoint_ClearOUT();

    while (Length)
    {
        uint8_t USB_DeviceState_LCL = state;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;
        if (Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;

        if (Endpoint_IsOUTReceived())
        {
            while (Length && Endpoint_BytesInEndpoint())
            {
                *DataStream = Endpoint_Read_8();
                DataStream += 1;
                Length--;
            }

            Endpoint_ClearOUT();
        }
    }

    while ((UEINTX & 1<<TXINI) == 0)
    {
        uint8_t USB_DeviceState_LCL = state;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;

        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

uint8_t Busby::Endpoint_Write_Control_PStream_LE(const void* const Buffer, uint16_t Length)
{
    uint8_t *DataStream = (uint8_t*)Buffer;
    bool LastPacketFull = false;

    if (Length > _ctrlReq.wLength)
        Length = _ctrlReq.wLength;
    else if (!(Length))
        Endpoint_ClearIN();

    while (Length || LastPacketFull)
    {
        uint8_t USB_DeviceState_LCL = state;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;

        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;

        if (Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;

        if (Endpoint_IsOUTReceived())
            break;

        if (UEINTX & 1<<TXINI)
        {
            uint16_t BytesInEndpoint = Endpoint_BytesInEndpoint();

            while (Length && (BytesInEndpoint < USB_Device_ControlEndpointSize))
            {
                Endpoint_Write_8(pgm_read_byte(DataStream));
                DataStream += 1;
                Length--;
                BytesInEndpoint++;
            }

            LastPacketFull = (BytesInEndpoint == USB_Device_ControlEndpointSize);
            Endpoint_ClearIN();
        }
    }

    while (!(Endpoint_IsOUTReceived()))
    {
        uint8_t USB_DeviceState_LCL = state;

        if (USB_DeviceState_LCL == DEVICE_STATE_UNATTACHED)
            return ENDPOINT_RWCSTREAM_DeviceDisconnected;
        if (USB_DeviceState_LCL == DEVICE_STATE_SUSPENDED)
            return ENDPOINT_RWCSTREAM_BusSuspended;
        if (Endpoint_IsSETUPReceived())
            return ENDPOINT_RWCSTREAM_HostAborted;
    }

    return ENDPOINT_RWCSTREAM_NoError;
}

void Busby::gen()
{
    if (*p_udint & 1<<sofi && *p_udien & 1<<sofe)
        UDINT &= ~(1<<SOFI);

    if (*p_usbint & 1<<vbusti && (USBCON & 1<<VBUSTE))
    {
        USBINT &= ~(1<<VBUSTI);

        if (USB_VBUS_GetStatus())
        {
            if (!(USB_Options & USB_OPT_MANUAL_PLL))
            {
                PLLCSR = USB_PLL_PSC;
                PLLCSR = USB_PLL_PSC | 1<<PLLE;

                while (USB_PLL_IsReady() == 0)
                    ;
            }

            state = DEVICE_STATE_POWERED;
        }
        else
        {
            if ((USB_Options & USB_OPT_MANUAL_PLL) == 0)
                PLLCSR = 0;

            state = DEVICE_STATE_UNATTACHED;
        }
    }

    if (*p_udint & 1<<suspi && (UDIEN & 1<<SUSPE))
    {
        UDIEN &= ~(1<<SUSPE);
        UDIEN |= 1<<WAKEUPE;
        USB_CLK_Freeze();

        if (!(USB_Options & USB_OPT_MANUAL_PLL))
            PLLCSR = 0;

        state = DEVICE_STATE_SUSPENDED;
    }

    if (*p_udint & 1<<wakeupi && (UDIEN & 1<<WAKEUPE))
    {
        if (!(USB_Options & USB_OPT_MANUAL_PLL))
        {
            PLLCSR = USB_PLL_PSC;
            PLLCSR = USB_PLL_PSC | 1<<PLLE;
            while (!(USB_PLL_IsReady()));
        }

        USBCON &= ~(1<<FRZCLK);
        UDINT &= ~(1<<WAKEUPI);
        UDIEN &= ~(1<<WAKEUPE);
        UDIEN |= 1<<SUSPE;

        if (USB_Device_ConfigurationNumber)
            state = DEVICE_STATE_Configured;
        else
            state = USB_Device_IsAddressSet() ? DEVICE_STATE_ADDRESSED :
                DEVICE_STATE_POWERED;
    }

    if (*p_udint & 1<<eorsti && (UDIEN & 1<<EORSTE))
    {
        UDINT &= ~(1<<EORSTI);
        state = DEVICE_STATE_Default;
        USB_Device_ConfigurationNumber = 0;
        UDINT &= ~(1<<SUSPI);
        UDIEN &= ~(1<<SUSPE);
        UDIEN |= 1<<WAKEUPE;
        Endpoint_ConfigureEndpoint(0, 0, USB_Device_ControlEndpointSize, 1);
    }
}

void Busby::USB_Device_SetAddress()
{
    uint8_t DeviceAddress = _ctrlReq.wValue & 0x7F;
    USB_Device_SetDeviceAddress(DeviceAddress);
    UEINTX &= ~(1<<RXSTPI);
    Endpoint_ClearStatusStage();

    while ((UEINTX & 1<<TXINI) == 0)
        ;

    USB_Device_EnableDeviceAddress(DeviceAddress);
    state = (DeviceAddress) ? DEVICE_STATE_ADDRESSED : DEVICE_STATE_Default;
}

void Busby::USB_Device_GetStatus()
{
    uint8_t CurrentStatus = 0;

    switch (_ctrlReq.bmRequestType)
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
        uint8_t EndpointIndex = ((uint8_t)_ctrlReq.wIndex & ENDPOINT_EPNUM_MASK);

        if (EndpointIndex >= ENDPOINT_TOTAL_ENDPOINTS)
            return;

        Endpoint_SelectEndpoint(EndpointIndex);
        CurrentStatus = Endpoint_IsStalled();
        Endpoint_SelectEndpoint(0);
    }
        break;
    default:
        return;
    }

    UEINTX &= ~(1<<RXSTPI);
    Endpoint_Write_16_LE(CurrentStatus);
    Endpoint_ClearIN();
    Endpoint_ClearStatusStage();
}

void Busby::USB_Init_Device()
{
    state = DEVICE_STATE_UNATTACHED;
    USB_Device_ConfigurationNumber = 0;
    USB_Device_RemoteWakeupEnabled = false;
    USB_Device_CurrentlySelfPowered = false;
    UDCON &= ~(1<<LSM);
    USBCON |= 1<<VBUSTE;
    Endpoint_ConfigureEndpoint(0, EP_TYPE_CONTROL, USB_Device_ControlEndpointSize, 1);
    UDINT &= ~(1<<SUSPI);
    UDIEN |= 1<<SUSPE;
    UDIEN |= 1<<EORSTE;
    UDCON &= ~(1<<DETACH);
}

void Busby::USB_ResetInterface()
{
    USBCON &= ~(1<<VBUSTE);
    UDIEN = 0;
    USBINT = 0;
    UDINT  = 0;
    USB_Controller_Reset();
    USBCON &= ~(1<<FRZCLK);
    PLLCSR = 0;
    USB_Init_Device();
    USBCON |= 1<<OTGPADE;
}

void Busby::Endpoint_ClearStatusStage()
{
    if (_ctrlReq.bmRequestType & REQDIR_DEVICETOHOST)
    {
        while ((Endpoint_IsOUTReceived()) == 0)
            if (state == DEVICE_STATE_UNATTACHED)
                return;

        Endpoint_ClearOUT();
    }
    else
    {
        while ((UEINTX & 1<<TXINI) == 0)
            if (state == DEVICE_STATE_UNATTACHED)
                return;

        Endpoint_ClearIN();
    }
}

void Busby::USB_Device_ClearSetFeature()
{
    switch (_ctrlReq.bmRequestType & CONTROL_REQTYPE_RECIPIENT)
    {
    case REQREC_DEVICE:
    {
        if ((uint8_t)_ctrlReq.wValue == FEATURE_SEL_DeviceRemoteWakeup)
            USB_Device_RemoteWakeupEnabled = (_ctrlReq.bRequest == REQ_SetFeature);
        else
            return;
    }
        break;
    case REQREC_ENDPOINT:
    {
        if ((uint8_t)_ctrlReq.wValue == FEATURE_SEL_EndpointHalt)
        {
            uint8_t EndpointIndex = ((uint8_t)_ctrlReq.wIndex & ENDPOINT_EPNUM_MASK);

            if (EndpointIndex == 0 || EndpointIndex >= ENDPOINT_TOTAL_ENDPOINTS)
              return;

            Endpoint_SelectEndpoint(EndpointIndex);

            if (UECONX & 1<<EPEN)
            {
                if (_ctrlReq.bRequest == REQ_SetFeature)
                {
                    UECONX |= 1<<STALLRQ;
                }
                else
                {
                    UECONX |= 1<<STALLRQC;
                    UERST = (1 << (EndpointIndex & ENDPOINT_EPNUM_MASK));
                    UERST = 0;
                    UECONX |= 1<<RSTDT;
                }
            }
        }
    }
        break;
    default:
        return;
    }

    Endpoint_SelectEndpoint(0);
    UEINTX &= ~(1<<RXSTPI);
    Endpoint_ClearStatusStage();
}

bool Busby::Endpoint_ConfigureEndpointTable(const USB_Endpoint_Table_t* const Table,
                                     const uint8_t Entries)
{
    for (uint8_t i = 0; i < Entries; i++)
    {
        if (!(Table[i].Address))
            continue;

        if (!(Endpoint_ConfigureEndpoint(Table[i].Address, Table[i].Type,
            Table[i].Size, Table[i].Banks)))
        {
            return false;
        }
    }

    return true;
}

void Busby::procCtrlReq(void)
{
    uint8_t* RequestHeader = (uint8_t*)&_ctrlReq;

    for (uint8_t i = 0; i < sizeof(USBRequest); i++)
        RequestHeader[i] = Endpoint_Read_8();

    customCtrl();

    if (*p_ueintx & 1<<rxstpi)
    {
        uint8_t bmRequestType = _ctrlReq.bmRequestType;

        switch (_ctrlReq.bRequest)
        {
        case REQ_GETSTATUS:
            if ((bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE)) ||
                (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_ENDPOINT)))
            {
                USB_Device_GetStatus();
            }

            break;
        case REQ_CLEARFEATURE:
        case REQ_SetFeature:
            if ((bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE)) ||
                (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_ENDPOINT)))
            {
                USB_Device_ClearSetFeature();
            }

            break;
        case REQ_SetAddress:
            if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
                USB_Device_SetAddress();

            break;
        case REQ_GetDescriptor:
            if ((bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE)) ||
                (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_INTERFACE)))
            {
                const void *descPtr;
                uint16_t DescriptorSize;

                if (_ctrlReq.wValue == ((DTYPE_STRING << 8) | USE_INTERNAL_SERIAL))
                {
                    USB_Device_GetInternalSerialDescriptor();
                    return;
                }
                if ((DescriptorSize = getDesc(_ctrlReq.wValue, _ctrlReq.wIndex, &descPtr)) == 0)
                {
                    return;
                }

                UEINTX &= ~(1<<RXSTPI);
                Endpoint_Write_Control_PStream_LE(descPtr, DescriptorSize);
                *p_ueintx &= ~(1<<rxouti | 1<<fifocon);
            }

            break;
        case REQ_GetConfiguration:
            if (bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE))
                USB_Device_GetConfiguration();

            break;
        case REQ_SETCONFIGURATION:
            if (bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_STANDARD | REQREC_DEVICE))
            {
                if ((uint8_t)_ctrlReq.wValue > FIXED_NUM_CONFIGURATIONS)
                    return;

                UEINTX &= ~(1<<RXSTPI);
                USB_Device_ConfigurationNumber = (uint8_t)_ctrlReq.wValue;
                Endpoint_ClearStatusStage();

                if (USB_Device_ConfigurationNumber)
                    state = DEVICE_STATE_Configured;
                else
                    state = USB_Device_IsAddressSet() ? DEVICE_STATE_Configured : DEVICE_STATE_POWERED;

                configure();
            }
            break;
        default:
            break;
        }
    }

    if (Endpoint_IsSETUPReceived())
    {
        UEINTX &= ~(1<<RXSTPI);
        UECONX |= 1<<STALLRQ;
    }
}



