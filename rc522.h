/*
*/
#include "types.h"
#include "stream.h"

const uint8_t MFRC522_firmware_referenceV0_0[] = {
  0x00, 0x87, 0x98, 0x0f, 0x49, 0xFF, 0x07, 0x19,
  0xBF, 0x22, 0x30, 0x49, 0x59, 0x63, 0xAD, 0xCA,
  0x7F, 0xE3, 0x4E, 0x03, 0x5C, 0x4E, 0x49, 0x50,
  0x47, 0x9A, 0x37, 0x61, 0xE7, 0xE2, 0xC6, 0x2E,
  0x75, 0x5A, 0xED, 0x04, 0x3D, 0x02, 0x4B, 0x78,
  0x32, 0xFF, 0x58, 0x3B, 0x7C, 0xE9, 0x00, 0x94,
  0xB4, 0x4A, 0x59, 0x5B, 0xFD, 0xC9, 0x29, 0xDF,
  0x35, 0x96, 0x98, 0x9E, 0x4F, 0x30, 0x32, 0x8D
};
// Version 1.0 (0x91)
// NXP Semiconductors; Rev. 3.8 - 17 September 2014; 16.1.1 self-test
const uint8_t MFRC522_firmware_referenceV1_0[] = {
  0x00, 0xC6, 0x37, 0xD5, 0x32, 0xB7, 0x57, 0x5C,
  0xC2, 0xD8, 0x7C, 0x4D, 0xD9, 0x70, 0xC7, 0x73,
  0x10, 0xE6, 0xD2, 0xAA, 0x5E, 0xA1, 0x3E, 0x5A,
  0x14, 0xAF, 0x30, 0x61, 0xC9, 0x70, 0xDB, 0x2E,
  0x64, 0x22, 0x72, 0xB5, 0xBD, 0x65, 0xF4, 0xEC,
  0x22, 0xBC, 0xD3, 0x72, 0x35, 0xCD, 0xAA, 0x41,
  0x1F, 0xA7, 0xF3, 0x53, 0x14, 0xDE, 0x7E, 0x02,
  0xD9, 0x0F, 0xB5, 0x5E, 0x25, 0x1D, 0x29, 0x79
};
// Version 2.0 (0x92)
// NXP Semiconductors; Rev. 3.8 - 17 September 2014; 16.1.1 self-test
const uint8_t MFRC522_firmware_referenceV2_0[] = {
  0x00, 0xEB, 0x66, 0xBA, 0x57, 0xBF, 0x23, 0x95,
  0xD0, 0xE3, 0x0D, 0x3D, 0x27, 0x89, 0x5C, 0xDE,
  0x9D, 0x3B, 0xA7, 0x00, 0x21, 0x5B, 0x89, 0x82,
  0x51, 0x3A, 0xEB, 0x02, 0x0C, 0xA5, 0x00, 0x49,
  0x7C, 0x84, 0x4D, 0xB3, 0xCC, 0xD2, 0x1B, 0x81,
  0x5D, 0x48, 0x76, 0xD5, 0x71, 0x61, 0x21, 0xA9,
  0x86, 0x96, 0x83, 0x38, 0xCF, 0x9D, 0x5B, 0x6D,
  0xDC, 0x15, 0xBA, 0x3E, 0x7D, 0x95, 0x3B, 0x2F
};
// Clone
// Fudan Semiconductor FM17522 (0x88)
const uint8_t FM17522_firmware_reference[] = {
  0x00, 0xD6, 0x78, 0x8C, 0xE2, 0xAA, 0x0C, 0x18,
  0x2A, 0xB8, 0x7A, 0x7F, 0xD3, 0x6A, 0xCF, 0x0B,
  0xB1, 0x37, 0x63, 0x4B, 0x69, 0xAE, 0x91, 0xC7,
  0xC3, 0x97, 0xAE, 0x77, 0xF4, 0x37, 0xD7, 0x9B,
  0x7C, 0xF5, 0x3C, 0x11, 0x8F, 0x15, 0xC3, 0xD7,
  0xC1, 0x5B, 0x00, 0x2A, 0xD0, 0x75, 0xDE, 0x9E,
  0x51, 0x64, 0xAB, 0x3E, 0xE9, 0x15, 0xB5, 0xAB,
  0x56, 0x9A, 0x98, 0x82, 0x26, 0xEA, 0x2A, 0x62
};

static constexpr uint8_t
    COMMAND_REG         = 0x01 << 1,  // starts and stops command execution
    ComIEnReg           = 0x02 << 1,  // enable and  control bits
    DivIEnReg           = 0x03 << 1,  // enable and disable interrupt request control bits
    ComIrqReg           = 0x04 << 1,  // interrupt request bits
    DivIrqReg           = 0x05 << 1,  // interrupt request bits
    ErrorReg            = 0x06 << 1,
    STATUS1REG          = 0x07 << 1,  // communication status bits
    STATUS2REG          = 0x08 << 1,  // receiver and transmitter status bits
    FIFODataReg         = 0x09 << 1,  // input and output of 64 byte FIFO buffer
    FIFOLevelReg        = 0x0A << 1,  // number of bytes stored in the FIFO buffer
    CONTROL_REG         = 0x0C << 1,  // miscellaneous control registers
    BitFramingReg       = 0x0D << 1,  // adjustments for bit-oriented frames
    CollReg             = 0x0E << 1,  // bit positionected on the RF interface
    MODE_REG            = 0x11 << 1,  // definansmitting and receiving 
    TX_MODE_REG         = 0x12 << 1,  // defines transmission data rate and framing
    RX_MODE_REG         = 0x13 << 1,  // defines reception data rate and framing
    TX_CONTROL_REG      = 0x14 << 1,
    TxASKReg            = 0x15 << 1,
    TxSelReg            = 0x16 << 1,
    RxSelReg            = 0x17 << 1,  // selects internal receiver settings
    RxThresholdReg      = 0x18 << 1,  // selects thresholds for the bit decoder
    DemodRe             = 0x19 << 1,  // defines demodulator settings
    MfTxReg             = 0x1C << 1,
    MfRxReg             = 0x1D << 1,
    CRCResultRegH       = 0x21 << 1,  // shows the  CRC calculation
    CRCResultRegL       = 0x22 << 1,
    ModWidthReg         = 0x24 << 1,  // controls the ModWidth setting?
    RFCfgReg            = 0x26 << 1,  // configures the receiver gain
    GsNReg              = 0x27 << 1,  // selects the 2 for modulation 
    CWGsPReg            = 0x28 << 1,  // defines the  periods of no modulation
    ModGsPReg           = 0x29 << 1,  // defines the uring periods of modulation
    TMODEREG            = 0x2A << 1,  // defines settings for the internal timer
    TPrescalerReg       = 0x2B << 1,  // the lower 8 gh bits are in TModeReg.
    TReloadRegH         = 0x2C << 1,  // defines the 16-bit timer reload value
    TReloadRegL         = 0x2D << 1,
    TCounterValueRegH   = 0x2E << 1,  // shows the 16-bit timer value
    TCounterValueRegL   = 0x2F << 1,
    TestSel1Reg         = 0x31 << 1,  // general test signal configuration
    TestSel2Reg         = 0x32 << 1,  // general test signal configuration
    TestPinEnReg        = 0x33 << 1,  // enables pin output driver on pins D1 to D7
    TestPinValueReg     = 0x34 << 1,  // defines the d as an I/O bus
    TestBusReg          = 0x35 << 1,  // shows the status of the internal test bus
    AutoTestReg         = 0x36 << 1,  // controls the digital self-test
    VersionReg          = 0x37 << 1,  // shows the software version
    AnalogTestReg       = 0x38 << 1,  // controls the pins AUX1 and AUX2
    TestDAC1Reg         = 0x39 << 1,  // defines the test value for TestDAC1
    TestDAC2Reg         = 0x3A << 1,  // defines the test value for TestDAC2
    TestADCReg          = 0x3B << 1,  // shows the value of ADC I and Q channels
    PCD_Idle            = 0x00,         // no action, cancels current command execution
    PCD_Mem             = 0x01,         // stores 25 bytes into the internal buffer
    PCD_GenerateRandomID     = 0x02,   // generates a 10-byte random ID number
    PCD_CalcCRC              = 0x03,   // activate coprocessor or performs a self-test
    PCD_Transmit             = 0x04,   // transmits data from the FIFO buffer
    PCD_NoCmdChange          = 0x07,   // no commad, for example, the PowerDown bit
    PCD_Receive              = 0x08,   // activates the receiver circuits
    PCD_TRANSCEIVE           = 0x0C,
    PCD_MFAuthent            = 0x0E,   // performsd authentication as a reader
    PCD_SoftReset            = 0x0F,   // resets the MFRC522
    PICC_CMD_REQA            = 0x26,   // REQuest command, Type A. Invites it frame.
    PICC_CMD_WUPA            = 0x52,   // Wake-UP command, Type A. Invites t frame.
    PICC_CMD_CT              = 0x88,   // Cascade Tag. Not really a commandllision.
    PICC_CMD_SEL_CL1         = 0x93,   // Anti collision/Select, Cascade Level 1
    PICC_CMD_SEL_CL2         = 0x95,   // Anti collision/Select, Cascade Level 2
    PICC_CMD_SEL_CL3         = 0x97,   // Anti collision/Select, Cascade Level 3
    PICC_CMD_HLTA            = 0x50,   // HaLT command, Type A. Instructs an
    PICC_CMD_RATS            = 0xE0,     // Request command for Answer To Reset.
    PICC_CMD_MF_AUTH_KEY_A   = 0x60,   // Perform authentication with Key A
    PICC_CMD_MF_AUTH_KEY_B   = 0x61,   // Perform authentication with Key B
    PICC_CMD_MF_READ         = 0x30,   // Reads one 16 byte blockIFARE Ultralight.
    PICC_CMD_MF_WRITE        = 0xA0,   // Writes one 16 byte blocor MIFARE Ultralight.
    PICC_CMD_MF_DECREMENT    = 0xC0,   // Decrements the contentsternal data register.
    PICC_CMD_MF_INCREMENT    = 0xC1,   // Increments the contentsernal data register.
    PICC_CMD_MF_RESTORE      = 0xC2,   // Reads the contents of ata register.
    PICC_CMD_MF_TRANSFER     = 0xB0,   // Writes the contents of ister to a block.
    PICC_CMD_UL_WRITE        = 0xA2,   // Writes one 4 byte page to the PICC.
    MF_ACK                   = 0xA,    // The MIFAother value than 0xA is NAK.
    MF_KEY_SIZE              = 6,      // A Mifare Crypto1 key is 6 bytes.
    PICC_TYPE_UNKNOWN        = 0,
    PICC_TYPE_ISO_14443_4    = 1,  // PICC compliant with ISO/IEC 14443-4 
    PICC_TYPE_ISO_18092      = 2,  // PICC compliant with ISO/IEC 18092 (NFC)
    PICC_TYPE_MIFARE_MINI    = 3,  // MIFARE Classic protocol, 320 bytes
    PICC_TYPE_MIFARE_1K      = 4,  // MIFARE Classic protocol, 1KB
    PICC_TYPE_MIFARE_4K      = 5,  // MIFARE Classic protocol, 4KB
    PICC_TYPE_MIFARE_UL      = 6,  // MIFARE Ultralight or Ultralight C
    PICC_TYPE_MIFARE_PLUS    = 7,  // MIFARE Plus
    PICC_TYPE_MIFARE_DESFIRE = 8, // MIFARE DESFire
    PICC_TYPE_TNP3XXX        = 9,
    PICC_TYPE_NOT_COMPLETE   = 0xff, // SAK indicates UID is not complete.
    STATUS_OK                = 0,  // Success
    STATUS_ERROR             = 1,  // Error in communication
    STATUS_COLLISION         = 2,  // Collission detected
    STATUS_TIMEOUT           = 3,  // Timeout in communication.
    STATUS_NO_ROOM           = 4,  // A buffer is not big enough.
    STATUS_INTERNAL_ERROR    = 5,  // Internal error in the code. Should not happen ;-)
    STATUS_INVALID           = 6,  // Invalid argument.
    STATUS_CRC_WRONG         = 7,  // The CRC_A does not match
    STATUS_MIFARE_NACK       = 0xff; // A MIFARE PICC responded with NAK.

class SPI
{
public:
    void init();
    void begin();
    void end();
    uint8_t transfer(uint8_t data);
};

struct Uid
{
    uint8_t size;
    uint8_t uidByte[10];
    uint8_t sak;
};

class MFRC522
{
    SPI _spi;
    Uid _uid;
public:
    void init();
    void antennaOn();
    void writeReg(uint8_t reg, uint8_t value);
    void writeReg(uint8_t reg, uint8_t count, uint8_t *values);
    uint8_t readReg(uint8_t reg);
    void readReg(uint8_t reg, uint8_t count, uint8_t *values, uint8_t rxAlign);
    void reset();
    bool isNewCardPresent();
    uint8_t requestA(uint8_t *bufferATQA, uint8_t *bufferSize);
    uint8_t reqaOrWupa(uint8_t command, uint8_t *bufferATQA, uint8_t *bufferSize);
    void setRegisterBitmask(uint8_t reg, uint8_t mask);
    void clearRegisterBitmask(uint8_t reg, uint8_t mask);
    uint8_t calculateCRC(uint8_t *data, uint8_t length, uint8_t *result);
    uint8_t select(Uid *uid, uint8_t validBits = 0);
    bool readCardSerial();
    void dump(ostream &os);
    bool selftest();

    uint8_t transceiveData(uint8_t *sendData, uint8_t sendLen, uint8_t *backData,
        uint8_t *backLen, uint8_t *validBits = nullptr, uint8_t rxAlign = 0,
        bool checkCRC = false);

    uint8_t communicateWithPICC(uint8_t command, uint8_t waitIRq,
            uint8_t *sendData, uint8_t sendLen, uint8_t *backData = nullptr,
            uint8_t *backLen = nullptr,
            uint8_t *validBits = nullptr, uint8_t rxAlign = 0, bool checkCRC = false);
};




