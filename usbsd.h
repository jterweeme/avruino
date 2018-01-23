#ifndef _USBSD2_H_
#define _USBSD2_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <avr/boot.h>
#include "busby.h"
#include "misc.h"
#include "zd2card.h"

static constexpr uint32_t
    M2S_CSW_SIGNATURE = 0x53425355,
    M2S_CBW_SIGNATURE = 0x43425355;

struct MS_KommandBlockWrapper_t
{
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataTransferLength;
    uint8_t Flags;
    uint8_t LUN;
    uint8_t SCSICommandLength;
    uint8_t SCSICommandData[16];
}
__attribute__ ((packed));

struct MS_KommandStatusWrapper_t
{
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataTransferResidue;
    uint8_t Status;
}
__attribute__ ((packed));

class USBSD : public USB
{
private:
    uint32_t _blocks;
    Board _board;
    Sd2Card _sd;
    Endpoint _inpoint;
    Endpoint _outpoint;
    MS_KommandBlockWrapper_t cmdBlock;
    MS_KommandStatusWrapper_t cmdStatus = { .Signature = M2S_CSW_SIGNATURE };
    bool ReadInCommandBlock();
    inline void _sdReadBlock(uint32_t block);
    void _sdReadBlocks(uint32_t start, uint16_t n);
    inline void _sdWriteBlock(uint32_t block);
    void _sdWriteBlocks(uint32_t start, uint16_t n);
    bool SCSI_Command_ReadWrite_10(const bool IsDataRead);
public:
    void ReturnCommandStatus();
    void MassStorage_Task();
    bool decodeSCSICmd();
    void procCtrlReq();
    USBSD();
};

#endif



