#ifndef _USBSD_H_
#define _USBSD_H_

#include "busby.h"
#include "zd2card.h"

static constexpr uint32_t
    M2S_CSW_SIGNATURE = 0x53425355,
    M2S_CBW_SIGNATURE = 0x43425355;

struct CmdBlock
{
    uint32_t signature;
    uint32_t tag;
    uint32_t DataTransferLength;
    uint8_t flags;
    uint8_t LUN;
    uint8_t SCSICommandLength;
    uint8_t cmdData[16];
}
__attribute__ ((packed));

struct CmdStatus
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
    CmdBlock cmdBlock;
    CmdStatus cmdStatus = { .Signature = M2S_CSW_SIGNATURE };
    bool ReadInCommandBlock();
    inline void _sdReadBlock(uint32_t block);
    void _sdReadBlocks(uint32_t start, uint16_t n);
    inline void _sdWriteBlock(uint32_t block);
    void _sdWriteBlocks(uint32_t start, uint16_t n);
    bool SCSI_Command_ReadWrite_10(const bool IsDataRead);
public:
    void customCtrl();
    uint16_t getDesc(uint16_t wValue, uint16_t wIndex, const void **const descAddr);
    void configure();
    void ReturnCommandStatus();
    void MassStorage_Task();
    bool decodeSCSICmd();
    USBSD();
};
#endif



