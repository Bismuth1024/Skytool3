#ifndef PN532_HEADER_GUARD
#define PN532_HEADER_GUARD

#include <stdint.h>
#include <memory.h>
#include "Interface.h"
#include "misc.h"
#include <fstream>


/*
Communication frames: (example)

Preamble 	0x00
Startcode	0x00
			0xFF
Length		0x04
Length CS	0x1C	"Negates" Length
TFI			0xD4	D4 host to PN532, D5 other way
Data		0x03	First is cmd code
			0x01
			0x02
Data CS		0x06	"Negates" TFI + sum of data
Postamble 	0x00

poly 0x8408
0x6363

"Useful data" starts at the cmd code.

*/
class MIFARE_1K;

class PN532 {
public:
    //A whole bunch of constants, e.g. communication protocols, command codes, etc
    static constexpr uint8_t PN532_BUFFER_SIZE = 0x40;
    static constexpr uint8_t PN532_I2C = 0x48 >> 1;

    static constexpr uint8_t PREAMBLE = 0x00;
    static constexpr uint8_t START1 = 0x00;
    static constexpr uint8_t START2 = 0xFF;
    static constexpr uint8_t POSTAMBLE = 0x00;

    static constexpr uint8_t Diagnose_CMD = 0x00;
    static constexpr uint8_t GetFirmwareVersion_CMD = 0x02;
    static constexpr uint8_t GetGeneralStatus_CMD = 0x04;
    static constexpr uint8_t ReadRegister_CMD = 0x06;
    static constexpr uint8_t WriteRegister_CMD = 0x08;
    static constexpr uint8_t ReadGPIO_CMD = 0x0C;
    static constexpr uint8_t WriteGPIO_CMD = 0x0E;
    static constexpr uint8_t SetSerialBaudRate_CMD = 0x10;
    static constexpr uint8_t SetParameters_CMD = 0x12;
    static constexpr uint8_t SAMConfiguration_CMD = 0x14;
    static constexpr uint8_t PowerDown_CMD = 0x16;

    static constexpr uint8_t RFConfiguration_CMD = 0x32;
    static constexpr uint8_t RFRegulationTest_CMD = 0x58;

    static constexpr uint8_t InJumpForDEP_CMD = 0x56;
    static constexpr uint8_t InJumpForPSL_CMD = 0x46;
    static constexpr uint8_t InListPassiveTarget_CMD = 0x4A;
    static constexpr uint8_t InATR_CMD = 0x50;
    static constexpr uint8_t InPSL_CMD = 0x4E;
    static constexpr uint8_t InDataExchange_CMD = 0x40;
    static constexpr uint8_t InCommunicateThru_CMD = 0x42;
    static constexpr uint8_t InDeselect_CMD = 0x44;
    static constexpr uint8_t InRelease_CMD = 0x52;
    static constexpr uint8_t InSelect_CMD = 0x54;
    static constexpr uint8_t InAutoPoll_CMD = 0x60;

    static constexpr uint8_t TgInitAsTarget_CMD = 0x8C;
    static constexpr uint8_t TgSetGeneralBytes_CMD = 0x92;
    static constexpr uint8_t TgGetData_CMD = 0x86;
    static constexpr uint8_t TgSetData_CMD = 0x8E;
    static constexpr uint8_t TgSetMetaData_CMD = 0x94;
    static constexpr uint8_t TgGetInitiatorCommand_CMD = 0x88;
    static constexpr uint8_t TgResponseToInitiator_CMD = 0x90;
    static constexpr uint8_t TgGetTargetStatus_CMD = 0x8A;

    static constexpr uint8_t PN532_ACK[6] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
    static constexpr uint8_t PN532_NACK[6] = {0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00};
    /*
     Constructs a PN532 object, to be communicated with over the given port.
     */
    PN532(Interface* port);
    /*

    setDebug: Toggles debugging messages

    d: whether to print debug messages

    */
    void setDebug(bool d);
    
    /*

    getFirmwareVersion: Prints the firmware version of the PN532.  Mainly used to check communication.

    */
    void getFirmwareVersion();
    
    /*
     
     */
    void SAMConfig();
            
    void setMuteTimeout(uint8_t timeout);
    void setCommunicationRetries(uint8_t retries);
    void setPassiveActivationRetries(uint8_t retries);
    
    void detectMifare1K(uint8_t uid[4]);
    void select(uint8_t tag);
    
    void MifareClassic_AuthenticateBlock(uint8_t block, uint8_t uid[4], bool isKeyA, uint8_t key[6]);
    void MifareClassic_ReadBlock(uint8_t block, uint8_t* destination);
    void MifareClassic_WriteBlock(uint8_t block, uint8_t data[16]);

    /*
     to revise the location of these
    void dumpFactoryMifare();
    void readMifare(uint8_t destination[0x40][0x10], uint8_t keys[0x10][0x06]);
    
    void skyInfo();
    
    void loadMagicMifare(const char* filename, uint8_t keys[0x10][0x06]);
     */
    
    class PN532Exception: public std::exception {
    private:
        
    public:
        uint8_t code;
        PN532Exception(uint8_t code);
        const char * what();
    };
    
private:
    Interface* port;
    uint8_t frameBuffer[PN532_BUFFER_SIZE];
    bool debug;
    
    void checkAck();
    void writeCommand(uint8_t len);
    void readData(uint8_t len);
    void decodeError(uint8_t error);
    void commandAndResponse(uint8_t commandLen, uint8_t responseLen);
    
    
    



};










#endif
