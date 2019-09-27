/*
rc522

code adapted from miguelbalboa
*/

#include "board.h"
#include "rc522.h"

#define F_CPU 16000000UL
#include <util/delay.h>

#ifndef DEBUG
//#define DEBUG
#endif

#ifdef DEBUG
extern ostream *g_debug;
#endif

void SPI::init()
{
    *p_ss_port |= 1<<ss_bit;
    *p_ss_ddr |= 1<<ss_bit;
    *p_spcr |= 1<<mstr | 1<<spe;
    *p_sck_ddr |= 1<<sck_bit;
    *p_mosi_ddr |= 1<<mosi_bit;
}

void SPI::begin()
{
    *p_spcr = 83;
    *p_spsr = 0;
}

void SPI::end()
{
}

uint8_t SPI::transfer(uint8_t data)
{
    *p_spdr = data;
    while ((*p_spsr & 1<<spif) == 0);
    return *p_spdr;
}

void MFRC522::writeReg(uint8_t reg, uint8_t value)
{
    _spi.begin();
    *p_pin10_port &= ~(1<<pin10_bit);    //CS low
    _spi.transfer(reg);
    _spi.transfer(value);
    *p_pin10_port |= 1<<pin10_bit;   //CS high
    _spi.end();
}

void MFRC522::writeReg(uint8_t reg, uint8_t count, uint8_t *values)
{
    _spi.begin();
    *p_pin10_port &= ~(1<<pin10_bit);    //CS low
    _spi.transfer(reg);

    for (uint8_t i = 0; i < count; i++)
        _spi.transfer(values[i]);

    *p_pin10_port |= 1<<pin10_bit;
    _spi.end();

}

uint8_t MFRC522::readReg(uint8_t reg)
{
    _spi.begin();
    *p_pin10_port &= ~(1<<pin10_bit);    //CS low
    _spi.transfer(0x80 | reg);
    uint8_t value = _spi.transfer(0);
    *p_pin10_port |= 1<<pin10_bit;   //CS high
    _spi.end();
    return value;
}

void MFRC522::readReg(uint8_t reg, uint8_t count, uint8_t *values, uint8_t rxAlign)
{
    if (count == 0)
        return;

    uint8_t address = 0x80 |reg;
    uint8_t index = 0;
    _spi.begin();
    *p_pin10_port &= ~(1<<pin10_bit);    //CS low
    count--;
    _spi.transfer(address);
    
    if (rxAlign)
    {
        uint8_t mask = (0xff << rxAlign) & 0xff;
        uint8_t value = _spi.transfer(address);
        values[0] = (values[0] & ~mask) | (value & mask);
        index++;
    }

    while (index < count)
    {
        values[index] = _spi.transfer(address);
        index++;
    }

    values[index] = _spi.transfer(0);
    *p_pin10_port |= 1<<pin10_bit;
    _spi.end();
}

void MFRC522::antennaOn()
{
    uint8_t value = readReg(TX_CONTROL_REG);

    if ((value & 0x03) != 0x03)
        writeReg(TX_CONTROL_REG, value | 0x03);
}

void MFRC522::init()
{
    _spi.init();
    bool hardReset = false;
    *p_pin10_ddr |= 1<<pin10_bit;   //CS pin output
    *p_pin10_port |= 1<<pin10_bit;  //CS pin HIGH
    *p_pin9_ddr &= ~(1<<pin9_bit);  //RST pin input

    if ((*p_pin9_in & 1<<pin9_bit) == 0) // if RST pin = 0
    {
        *p_pin9_ddr |= 1<<pin9_bit; //RST pin output
        *p_pin9_port |= 1<<pin9_bit;    //RST pin = 0; triggers hard reset
        _delay_ms(50);
        hardReset = true;
    }

    if (!hardReset)
        reset();
    
    writeReg(TX_MODE_REG, 0);
    writeReg(RX_MODE_REG, 0);
    writeReg(ModWidthReg, 0x26);
    writeReg(TMODEREG, 0x80);
    writeReg(TPrescalerReg, 0xa9);
    writeReg(TReloadRegH, 0x03);
    writeReg(TReloadRegL, 0xe8);
    writeReg(TxASKReg, 0x40);
    writeReg(MODE_REG, 0x3d);
    antennaOn();
}

void MFRC522::reset()
{
    writeReg(COMMAND_REG, PCD_SoftReset);
    
    uint8_t count = 0;
    do {
        _delay_ms(50);
    } while ((readReg(COMMAND_REG) & (1<<4)) && (++count) < 3);
}

uint8_t MFRC522::reqaOrWupa(uint8_t command, uint8_t *bufferATQA, uint8_t *bufferSize)
{
    if (bufferATQA == nullptr || *bufferSize < 2)
        return STATUS_NO_ROOM;

    clearRegisterBitmask(CollReg, 0x80);
    uint8_t validBits = 7;
    uint8_t status = transceiveData(&command, 1, bufferATQA, bufferSize, &validBits);

    if (status != STATUS_OK)
        return status;

    if (*bufferSize != 2 || validBits != 0)
        return STATUS_ERROR;

    return STATUS_OK;
}

uint8_t MFRC522::requestA(uint8_t *bufferATQA, uint8_t *bufferSize) {
    return reqaOrWupa(PICC_CMD_REQA, bufferATQA, bufferSize);
}

bool MFRC522::isNewCardPresent()
{
#ifdef DEBUG
    //g_debug->dinges("IsNewCardPresent()\r\n");
#endif
    uint8_t bufferATQA[2];
    uint8_t bufferSize = sizeof(bufferATQA);
    writeReg(TX_MODE_REG, 0);
    writeReg(RX_MODE_REG, 0);
    writeReg(ModWidthReg, 0x26);
    uint8_t result = requestA(bufferATQA, &bufferSize);
#ifdef DEBUG
    g_debug->dinges("requestA: ");
    hex8(result, *g_debug);
    g_debug->dinges("\r\n");
#endif
    return (result == 0 || result == 2);
}

void MFRC522::setRegisterBitmask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp = readReg(reg);
    writeReg(reg, tmp | mask);
}

void MFRC522::clearRegisterBitmask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp = readReg(reg);
    writeReg(reg, tmp & ~mask);
}

uint8_t MFRC522::transceiveData(uint8_t *sendData, uint8_t sendLen, uint8_t *backData,
        uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign, bool checkCRC)
{
    return communicateWithPICC(PCD_TRANSCEIVE, 0x30, sendData, sendLen,
        backData, backLen, validBits, rxAlign, checkCRC);
}

uint8_t MFRC522::calculateCRC(uint8_t *data, uint8_t length, uint8_t *result)
{
#ifdef DEBUG
    g_debug->dinges("CalculateCRC\r\n");
#endif
    writeReg(COMMAND_REG, PCD_Idle);
    writeReg(DivIrqReg, 4);
    writeReg(FIFOLevelReg, 0x80);
    writeReg(FIFODataReg, length, data);
    writeReg(COMMAND_REG, PCD_CalcCRC);

    for (uint16_t i = 5000; i > 0; i--)
    {
        uint8_t n = readReg(DivIrqReg);
        if (n & 4)
        {
            writeReg(COMMAND_REG, PCD_Idle);
            result[0] = readReg(CRCResultRegL);
            result[1] = readReg(CRCResultRegH);
            return STATUS_OK;
        }
    }
    return STATUS_TIMEOUT;
}

uint8_t MFRC522::select(Uid *uid, uint8_t validBits)
{
    uint8_t index;
    int8_t currentLevelKnownBits;

    if (validBits > 80)
        return STATUS_INVALID;

    clearRegisterBitmask(CollReg, 0x80);
    bool uidComplete = false, useCascadeTag;
    uint8_t cascadeLevel = 1, rxAlign, *responseBuffer, responseLength, checkBit;
    uint8_t buffer[9], bufferUsed, uidIndex, count, result, txLastBits;

    while (!uidComplete)
    {
        switch (cascadeLevel)
        {
        case 1:
            buffer[0] = PICC_CMD_SEL_CL1;
            uidIndex = 0;
            useCascadeTag = validBits && uid->size > 4;
            break;
        case 2:
            buffer[0] = PICC_CMD_SEL_CL2;
            uidIndex = 3;
            useCascadeTag = validBits && uid->size > 7;
            break;
        case 3:
            buffer[0] = PICC_CMD_SEL_CL3;
            uidIndex = 6;
            useCascadeTag = false;
            break;
        default:
            return STATUS_INTERNAL_ERROR;
        }

        currentLevelKnownBits = validBits - (8 * uidIndex);

        if (currentLevelKnownBits < 0)
            currentLevelKnownBits = 0;

        index = 2;

        if (useCascadeTag)
            buffer[index++] = PICC_CMD_CT;

        uint8_t bytesToCopy = currentLevelKnownBits / 8 + (currentLevelKnownBits % 8 ? 1 : 0);
        if (bytesToCopy)
        {
            uint8_t maxBytes = useCascadeTag ? 3 : 4;

            if (bytesToCopy > maxBytes)
                bytesToCopy = maxBytes;

            for (count = 0; count < bytesToCopy; count++)
                buffer[index++] = uid->uidByte[uidIndex + count];
        }

        if (useCascadeTag)
            currentLevelKnownBits += 8;

        bool selectDone = false;
        while (!selectDone)
        {
            if (currentLevelKnownBits >= 32)
            {
                buffer[1] = 0x70;
                buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
                result = calculateCRC(buffer, 7, &buffer[7]);

                if (result != STATUS_OK)
                    return result;

                txLastBits = 0;
                bufferUsed = 9;
                responseBuffer = &buffer[6];
                responseLength = 3;
            }
            else
            {
                txLastBits = currentLevelKnownBits % 8;
                count = currentLevelKnownBits / 8;
                index = 2 + count;
                buffer[1] = (index << 4) + txLastBits;
                bufferUsed = index + (txLastBits ? 1 : 0);
                responseBuffer = &buffer[index];
                responseLength = sizeof(buffer) - index;
            }

            rxAlign = txLastBits;
            writeReg(BitFramingReg, (rxAlign << 4) + txLastBits);
            
            result = transceiveData(buffer, bufferUsed, responseBuffer,
                        &responseLength, &txLastBits, rxAlign);

            if (result == STATUS_COLLISION)
            {
                uint8_t valueOfCollReg = readReg(CollReg);

                if (valueOfCollReg & 0x20)
                    return STATUS_COLLISION;

                uint8_t collisionPos = valueOfCollReg & 0x1f;

                if (collisionPos == 0)
                    collisionPos = 32;

                if (collisionPos <= currentLevelKnownBits)
                    return STATUS_INTERNAL_ERROR;

                currentLevelKnownBits = collisionPos;
                count = currentLevelKnownBits % 8;
                checkBit = (currentLevelKnownBits - 1) % 8;
                index = 1 + (currentLevelKnownBits / 8) + (count ? 1 : 0);
                buffer[index] |= 1<<checkBit;
            }
            else if (result != STATUS_OK)
            {
                return result;
            }
            else
            {
                if (currentLevelKnownBits >= 32)
                    selectDone = true;
                else
                    currentLevelKnownBits = 32;
            }
        }

        index = (buffer[2] == PICC_CMD_CT) ? 3 : 2;
        bytesToCopy = (buffer[2] == PICC_CMD_CT) ? 3 : 4;

        for (count = 0; count < bytesToCopy; count++)
            uid->uidByte[uidIndex + count] = buffer[index++];

        if (responseLength != 3 || txLastBits != 0)
            return STATUS_ERROR;

        result = calculateCRC(responseBuffer, 1, &buffer[2]);
        if (result != STATUS_OK)
            return result;

        if ((buffer[2] != responseBuffer[1]) || (buffer[3] != responseBuffer[2]))
            return STATUS_CRC_WRONG;

        if (responseBuffer[0] & 4)
        {
            cascadeLevel++;
        }
        else
        {
            uidComplete = true;
            uid->sak = responseBuffer[0];
        }
    }
    uid->size = 3 * cascadeLevel + 1;
    return STATUS_OK;
}

uint8_t MFRC522::communicateWithPICC(uint8_t command, uint8_t waitIRq,
            uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen,
            uint8_t *validBits, uint8_t rxAlign, bool checkCRC)
{
#ifdef DEBUG
    g_debug->dinges("CommunicateWithPICC()\r\n");
#endif
    uint8_t txLastBits = validBits ? *validBits : 0;
    uint8_t bitFraming = (rxAlign << 4) + txLastBits;
    writeReg(COMMAND_REG, PCD_Idle);
    writeReg(ComIrqReg, 0x7f);
    writeReg(FIFOLevelReg, 0x80);
    writeReg(FIFODataReg, sendLen, sendData);
    writeReg(BitFramingReg, bitFraming);
    writeReg(COMMAND_REG, command);

    if (command == PCD_TRANSCEIVE)
        setRegisterBitmask(BitFramingReg, 0x80);


    uint16_t i;
    for (i = 2000; i > 0; i--)
    {
        uint8_t n = readReg(ComIrqReg);
        
        if (n & waitIRq)
            break;

        if (n & 0x01)
            return STATUS_TIMEOUT;
    }

    if (i == 0)
        return STATUS_TIMEOUT;

    uint8_t errorRegValue = readReg(ErrorReg);

    if (errorRegValue & 0x13)
        return STATUS_ERROR;

    uint8_t validBits2 = 0;
    
    if (backData && backLen)
    {
        uint8_t n = readReg(FIFOLevelReg);

        if (n > *backLen)
            return STATUS_NO_ROOM;
        
        *backLen = n;
        readReg(FIFODataReg, n, backData, rxAlign);
        validBits2 = readReg(CONTROL_REG) & 0x07;

        if (validBits)
            *validBits = validBits2;
    }

#ifdef DEBUG
    g_debug->dinges("Debug bericht\r\n");
#endif

    if (errorRegValue & 0x08)
        return STATUS_COLLISION;

    if (backData && backLen && checkCRC)
    {
        if (*backLen == 1 && validBits2 == 4)
            return STATUS_MIFARE_NACK;

        if (*backLen < 2 || validBits2 != 0)
            return STATUS_CRC_WRONG;

        uint8_t controlBuffer[2];
        uint8_t status = calculateCRC(&backData[0], *backLen - 2, &controlBuffer[0]);

        if (status != STATUS_OK)
            return status;

        if ((backData[*backLen - 2] != controlBuffer[0]) ||
            (backData[*backLen - 1] != controlBuffer[1]))
        {
            return STATUS_CRC_WRONG;
        }
    }

    return STATUS_OK;
}

bool MFRC522::selftest()
{
    reset();
    uint8_t zeroes[25] = {0};
    writeReg(FIFOLevelReg, 0x80);
    writeReg(FIFODataReg, 25, zeroes);
    writeReg(COMMAND_REG, PCD_Mem);
    writeReg(AutoTestReg, 9);
    writeReg(FIFODataReg, 0);
    writeReg(COMMAND_REG, PCD_CalcCRC);

    // TODO: de rest
    return 0;
}

bool MFRC522::readCardSerial()
{
    uint8_t result = select(&_uid);
    return (result == STATUS_OK);
}

void MFRC522::dump(ostream &os)
{
    os << "Kaarttype: ";
    hex8(_uid.sak, os);
    os.flush();
}


