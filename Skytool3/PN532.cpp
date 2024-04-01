#include "PN532.h"


PN532::PN532(Interface* port) : port(port), debug(false) { }

void PN532::setDebug(bool d) {
	debug = d;
}

void PN532::getFirmwareVersion() {
	frameBuffer[0] = GetFirmwareVersion_CMD;
	
    commandAndResponse(1, 5);
    
    std::cout << "A PN5" << std::hex << std::setw(1) << frameBuffer[1] << " was found, version: " << std::dec << frameBuffer[2] << "." << frameBuffer[3] << std::endl;
    
    std::cout << "Supports code: " << std::hex << frameBuffer[4] << std::endl;
}

//THIS MUST BE CALLED BEFORE DOING ANYTHING
void PN532::SAMConfig() {

	frameBuffer[0] = SAMConfiguration_CMD;
	frameBuffer[1] = 0x01;
	frameBuffer[2] = 0x14;
	frameBuffer[3] = 0x01;
	
    commandAndResponse(4, 1);
    std::cout << "SAM was successfully configured." << std::endl;
}

/*

Description: Sets how long the PN532 will wait for a card response.

Arguments:	Timeout value (see datasheet for conversions)

Returns: Success boolean

*/


void PN532::setMuteTimeout(uint8_t timeout) {

	if (timeout > 0x10) timeout = 0x10;
		
	frameBuffer[0] = RFConfiguration_CMD;
	frameBuffer[1] = 0x02; //Config Item
	frameBuffer[2] = 0x00; //RFU
	frameBuffer[3] = 0x0B; //Dont alter
	frameBuffer[4] = timeout;
	
    commandAndResponse(5, 1);
}

/*

Description: Sets how many times the PN532 will attempt to communicate with target before giving up.

Arguments:	Number of retries (NOT ATTEMPTS)

Returns: Success boolean

*/

void PN532::setCommunicationRetries(uint8_t retries) {
		
	frameBuffer[0] = RFConfiguration_CMD;
	frameBuffer[1] = 0x04; //Config Item
	frameBuffer[2] = retries; 
	
    commandAndResponse(3, 1);
}

/*

Description: Sets how many times the PN532 will attempt to active passive targets.

Arguments:	Number of retries (NOT ATTEMPTS)

Returns: Success boolean

*/

void PN532::setPassiveActivationRetries(uint8_t retries) {
		
	frameBuffer[0] = RFConfiguration_CMD;
	frameBuffer[1] = 0x05; //Config Item
	frameBuffer[2] = 0xFF; //Dont alter
	frameBuffer[3] = 0x01; //Dont alter
	frameBuffer[4] = retries;
	
    commandAndResponse(5, 1);
}

/*

Description: Detects a MIFARE 1K card in the RF field and saves its UID.

Arguments:	uid - destination for the UID.

Returns: Success boolean

*/

void PN532::detectMifare1K(uint8_t uid[4]) {
    
    frameBuffer[0] = InListPassiveTarget_CMD;
    frameBuffer[1] = 0x01; //1 Tag
    frameBuffer[2] = 0x00; //106 kbps type A
    
    commandAndResponse(3, 11);
    
    //Currently only supports one tag
    if (frameBuffer[1] != 0x01) throw CodedException(0x0A);
    
    memcpy(uid, frameBuffer + 7, 4);
    
    std::cout << "Found a tag with UID " << std::endl;
    printHexBytes(uid, 4, -1);
    std::cout << std::hex << std::setw(2) << std::setfill('0') << "ATQA of " << frameBuffer[3] << " " << frameBuffer[4] << ", SAK of " << frameBuffer[5] << std::endl;
}

/*

Description: Selects a target.

Arguments:	Tag number (as defined by PN532)

Returns: Success boolean

*/

void PN532::select(uint8_t tag) {

	frameBuffer[0] = InSelect_CMD;
	frameBuffer[1] = tag;
	
    commandAndResponse(2, 2);
	
    decodeError(frameBuffer[1]);
}

/*

Description: Authenticates a sector for a MIFARE Classic 1K card.

Arguments:	block - The block (NOT SECTOR) to authenticate (but it will authenticate the whole sector)
			uid - The UID of the target to authenticate
			keyType - Which authentication type: Key A is true, Key B is false
			key - The key to use

Returns: Success boolean

*/

void PN532::MifareClassic_AuthenticateBlock(uint8_t block, uint8_t uid[4], bool isKeyA, uint8_t key[6]) {
	
	//True is for keyA, false is for keyB
	frameBuffer[0] = InDataExchange_CMD;
	frameBuffer[1] = 0x01; //Tag Number
	frameBuffer[2] = (isKeyA ? 0x60 : 0x61); //Key A or Key B Auth
	frameBuffer[3] = block;
	memcpy(frameBuffer + 4, key, 6);
	memcpy(frameBuffer + 10, uid, 4);
	
    commandAndResponse(14, 2);
	
    decodeError(frameBuffer[1]);
}

/*

Description: Reads a block for for a MIFARE Classic 1K card.

Arguments:	block - Number of the block to be read
			destination - Destination for the data read

Returns: Success boolean

*/

void PN532::MifareClassic_ReadBlock(uint8_t block, uint8_t* destination) {
	
	frameBuffer[0] = InDataExchange_CMD;
	frameBuffer[1] = 0x01; //Tag Number
	frameBuffer[2] = 0x30; //Mifare read
	frameBuffer[3] = block;
	
    commandAndResponse(4, 18);
		
	memcpy(destination, frameBuffer + 2, 16);
	
    decodeError(frameBuffer[1]);
}

/*

Description: Writes a block for a MIFARE Classic 1K card.

Arguments:	block - Number of the block to be written
			data - The data to write

Returns: Success boolean

*/

void PN532::MifareClassic_WriteBlock(uint8_t block, uint8_t data[16]) {

	frameBuffer[0] = InDataExchange_CMD;
	frameBuffer[1] = 0x01;
	frameBuffer[2] = 0xA0; //Mifare write
	frameBuffer[3] = block;
	
	memcpy(frameBuffer + 4, data, 16);
	
    commandAndResponse(20, 2);

	decodeError(frameBuffer[1]);
}

/*

Description: Reads an entire MIFARE Classic card (given the keys)

Arguments:	destination - Destination for the read data.
			keys - Keys to use.

Returns: Success boolean

*/

/*
void PN532::readMifare(uint8_t destination[0x40][0x10], uint8_t keys[0x10][0x06]) {
	uint8_t uid[4];
	
    detectMifare1K(uid); //Look for a MIFARE 1K card
    select(0x01); //Select it
	
	for (uint8_t block = 0; block < 0x40; block++) {
		if (isFirstBlock(block)) {
			if (!MifareClassic_AuthenticateBlock(block, uid, true, keys[block/4])) return false;
		}

		if (!MifareClassic_ReadBlock(block, destination[block])) return false;	
		
		if (isTrailerBlock(block)) {
			//since keyA not readable, the card will return all zeroes so we must fill in the real data
			memcpy(destination[block], keys[block/4], 0x06);
		}
	}
	
	return true;
}
 */

/*

Description: Attempts to read an acknowledge frame from the PN532

Arguments:	

Returns: none

*/

void PN532::checkAck() {
	static uint8_t ackbuff[7];
	
    port->receiveI2C(PN532_I2C, ackbuff, 7);
	
	if (debug) {
		if (memcmp(ackbuff + 1, PN532_ACK, 6) == 0) {
			std::cout << "PN532 acknowledged.\n" << std::endl;
		} else {
			std::cout << "Incorrect ACK received.\n" << std::endl;
		}
	}
	
    if (memcmp(ackbuff + 1, PN532_ACK, 6)) throw CodedException(0x09);
}

/*

Description: Sends a command (TFI to PDN) to the PN532 (the command is in the frameBuffer)

Arguments:	len - How many bytes are in the command

Returns: Success boolean - If the PN532 ack'd or not

*/

void PN532::writeCommand(uint8_t len) {

	static uint8_t cmdBuffer[64];

	cmdBuffer[0] = PREAMBLE;
	cmdBuffer[1] = START1;
	cmdBuffer[2] = START2;
	cmdBuffer[3] = len + 1; //TFI to PDn
	cmdBuffer[4] = ~(cmdBuffer[3]) + 1; //LCS
	cmdBuffer[5] = 0xD4; //Direction
	memcpy(cmdBuffer + 6, frameBuffer, len);
	
	uint8_t dataChecksum = 0xD4;

	for (uint8_t i = 0; i < len; i++) {
		dataChecksum += cmdBuffer[6 + i];
	}
	
	dataChecksum = ~dataChecksum;
	dataChecksum++;
	
	cmdBuffer[6 + len] = dataChecksum;
	cmdBuffer[7 + len] = POSTAMBLE;
	
	if (debug) {
		std::cout << "Sending the following command to PN532: " << std::endl;
		printHexBytes(frameBuffer, len, -1);
	}
	
    port->sendI2C(PN532_I2C, cmdBuffer, len + 8);
	
    checkAck();
}

/*

Description: Reads a response frame from the PN532 and stores TFI to PDN in the frameBuffer.

Arguments:	len - The number of bytes to read

Returns: Success boolean

*/

void PN532::readData(uint8_t len) {
	static uint8_t responseBuffer[64];
	
	//Leading 0x01 if ready for I2C!
    port->receiveI2C(PN532_I2C, responseBuffer, len + 8);
	
	if (debug) {
		std::cout << "Received this data from the PN532: " << std::endl;
		printHexBytes(responseBuffer + 7, len, -1);
	}
	
	if (responseBuffer[6] != 0xD5) {
		//If this is not the TFI, then its probably an error
		decodeError(responseBuffer[6]);
	}
	
	memcpy(frameBuffer, responseBuffer + 7, len); //Copy the actually useful data
}

void PN532::commandAndResponse(uint8_t commandLen, uint8_t responseLen) {
    writeCommand(commandLen);
    readData(responseLen);
}

/*

bool PN532::loadMagicMifare(const char* filename, uint8_t keys[0x10][0x06]) {
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	char buffer[0x400];
	uint8_t data[0x40][0x10];
	file.read(buffer, 0x400);
	file.close();
	
	uint8_t uid[4];
	detectMifare1K(uid);
	select(0x01);
	
	memcpy(data, buffer, 0x400);

	for (uint8_t block = 0; block < 0x40; block++) {
		if (isFirstBlock(block)) {
			if (!MifareClassic_AuthenticateBlock(block, uid, true, keys[block/4])) return false;
		}
		MifareClassic_WriteBlock(block, data[block]);
		
	}
	
	return true;
}
 */

void PN532::decodeError(uint8_t error) {
	if (error == 0x00) return;
    
    throw PN532Exception(error);
}


PN532::PN532Exception::PN532Exception(uint8_t code) : code(code) {}

const char * PN532::PN532Exception::what() {
    std::stringstream stream;
    stream << std::hex << std::setw(2) << std::setfill('0') << "Error " << code << ": ";
    
    switch (code) {
        case 0x01:
            stream << "Time Out, the target has not answered\n";
            break;
        case 0x02:
            stream << "A CRC error has been detected by the CIU";
            break;
        case 0x03:
            stream << "A Parity error has been detected by the CIU\n";
            break;
        case 0x04:
            stream << "During an anti-collision/select operation (ISO/IEC14443-3 Type A and ISO/IEC18092 106 kbps passive mode), an erroneous Bit Count has been detected\n";
            break;
        case 0x05:
            stream << "Framing error during Mifare operation\n";
            break;
        case 0x06:
            stream << "An abnormal bit-collision has been detected during bit wise anti-collision at 106 kbps\n";
            break;
        case 0x07:
            stream << "Communication buffer size insufficient\n";
            break;
        case 0x09:
            stream << "RF Buffer overflow has been detected by the CIU (bit BufferOvfl of the register CIU_Error)\n";
            break;
        case 0x0A:
            stream << "In active communication mode, the RF field has not been switched on in time by the counterpart (as defined in NFCIP-1 standard)\n";
            break;
        case 0x0B:
            stream << "RF Protocol error (cf. Error! Reference source not found., description of the CIU_Error register)\n";
            break;
        case 0x0D:
            stream << "Temperature error: the internal temperature sensor has detected overheating, and therefore has automatically switched off the antenna drivers\n";
            break;
        case 0x0E:
            stream << "Internal buffer overflow\n";
            break;
        case 0x10:
            stream << "Invalid parameter (range, format, ...)\n";
            break;
        case 0x12:
            stream << "DEP Protocol: The PN532 configured in target mode does not support the command received from the initiator (the command received is not one of the following: ATR_REQ, WUP_REQ, PSL_REQ, DEP_REQ, DSL_REQ, RLS_REQ)\n";
            break;
        case 0x13:
            stream << "DEP Protocol, Mifare or ISO/IEC14443-4: The data format does not match to the specification.  Depending on the RF protocol used, it can be: Bad length of RF received frame, Incorrect value of PCB or PFB, Invalid or unexpected RF received frame, NAD or DIDincoherence.\n";
            break;
        case 0x14:
            stream << "MIFARE Authentication error.\n";
            break;
        case 0x23:
            stream << "\n";
            break;
        case 0x25:
            stream << "\n";
            break;
        case 0x26:
            stream << "\n";
            break;
        case 0x27:
            stream << "\n";
            break;
        case 0x29:
            stream << "\n";
            break;
        case 0x2A:
            stream << "\n";
            break;
        case 0x2B:
            stream << "\n";
            break;
        case 0x2C:
            stream << "\n";
            break;
        case 0x2D:
            stream << "\n";
            break;
        case 0x2E:
            stream << "\n";
            break;
        default:
            stream << "UNKNOWN";

    }
    
    return stream.str().c_str();
}
