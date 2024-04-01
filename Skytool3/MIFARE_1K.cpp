#include "MIFARE_1K.h"


/*
*******************************************************************************
*******************************************************************************
CONSTRUCTORS*******************************************************************
*******************************************************************************
*******************************************************************************
*/

MIFARE_1K::MIFARE_1K() {
    setDefault();
	paramsToData();
}

MIFARE_1K::MIFARE_1K(const char* filename) {
	readFile(filename, &data[0][0], 0x400);
	dataToParams(); //Copy in all the keys, UID, etc. to their own variables
	memset(altered, 0x00, 0x40);
}

MIFARE_1K::MIFARE_1K(const uint8_t dataIn[0x40][0x10]) {
    for (uint8_t block = 0; block < 0x40; block++) {
        memcpy(data[block], dataIn[block], 0x10);
    }
    dataToParams(); //Copy in all the keys, UID, etc. to their own variables
    memset(altered, 0x00, 0x40);
}

MIFARE_1K::MIFARE_1K(PN532* pn532) {
    setDefault();
    pn532->SAMConfig();
    pn532->detectMifare1K(UID);
    memcpy(data[0x00], UID, 0x04);
    calcBCC();
}

/*
*******************************************************************************
*******************************************************************************
GET/SET************************************************************************
*******************************************************************************
*******************************************************************************
*/

void MIFARE_1K::setDefault() {
    isMagic = false;
    memset(UID, 0x00, 0x04);
    memset(data, 0x00, 0x400); //Clear data
    memset(altered, 0x00, 0x40);
    memcpy(&data[0][5], defaultZero, 0x0B);
    for (uint8_t sector = 0; sector < 0x10; sector++) {
        setKeyA(sector, defaultKey);
        setKeyB(sector, defaultKey);
        setAccessBits(sector, defaultAccessBits);
        setSpareByte(sector, &defaultSpareByte);
    }
}

void MIFARE_1K::setBytes(uint8_t block, uint8_t offset, const uint8_t* dataIn, uint8_t len) {
    if (!MIFARE_1K::MIFARE_1K::isDataBlock(block)) throw CodedException(0x0B);
    if (offset + len > 0x10) throw CodedException(0x0B);
    if (!isMagic && block == 0x00) throw CodedException(0x0C);
	
	memcpy(&data[block][offset], dataIn, len);
	flag(block);
}

void MIFARE_1K::getBytes(uint8_t block, uint8_t offset, uint8_t* destination, uint8_t len) {
    if (offset + len > 0x10) throw CodedException(0x0B);

	memcpy(destination, &data[block][offset], len);
}

void MIFARE_1K::setBlock(uint8_t block, const uint8_t blockIn[0x10]) {
    memcpy(data[block], blockIn, 0x10);
    flag(block);
}

void MIFARE_1K::getBlock(uint8_t block, uint8_t destination[0x10]) {
    memcpy(destination, data[block], 0x10);
}

void MIFARE_1K::getUID(uint8_t destination[0x04]) {
	memcpy(destination, UID, 0x04);
}

void MIFARE_1K::setUID(const uint8_t newUID[0x04]) {
    memcpy(UID, newUID, 0x04);
}

void MIFARE_1K::getKeyA(uint8_t sector, uint8_t destination[0x06]) {
    if (!MIFARE_1K::isValidSector(sector)) throw CodedException(0x0B);
	memcpy(destination, keysA[sector], 0x06);
}

void MIFARE_1K::setKeyA(uint8_t sector, const uint8_t* key) {
    if (!MIFARE_1K::isValidSector(sector)) throw CodedException(0x0B);
	memcpy(keysA[sector], key, 0x06);
}

void MIFARE_1K::getKeyB(uint8_t sector, uint8_t destination[0x06]) {
    if (!MIFARE_1K::isValidSector(sector)) throw CodedException(0x0B);
	memcpy(destination, keysB[sector], 0x06);
}

void MIFARE_1K::setKeyB(uint8_t sector, const uint8_t key[0x06]) {
    if (!MIFARE_1K::isValidSector(sector)) throw CodedException(0x0B);
	memcpy(keysB[sector], key, 0x06);
}

void MIFARE_1K::getAccessBits(uint8_t sector, uint8_t destination[0x03]) {
    if (!MIFARE_1K::isValidSector(sector)) throw CodedException(0x0B);
	memcpy(destination, accessBits[sector], 0x03);
}

void MIFARE_1K::setAccessBits(uint8_t sector, const uint8_t bits[0x03]) {
    if (!MIFARE_1K::isValidSector(sector)) throw CodedException(0x0B);
	memcpy(accessBits[sector], bits, 0x03);
}

void MIFARE_1K::getSpareByte(uint8_t sector, uint8_t* destination) {
    if (!MIFARE_1K::isValidSector(sector)) throw CodedException(0x0B);	*(destination) = spareBytes[sector];
}

void MIFARE_1K::setSpareByte(uint8_t sector, const uint8_t* newByte) {
    if (!MIFARE_1K::isValidSector(sector)) throw CodedException(0x0B);
	spareBytes[sector] = *(newByte);
}

void MIFARE_1K::getData(uint8_t destination[0x300]) {
    uint8_t *ptr = destination;
    for (uint8_t block = 0; block < 0x40; block++) {
        if (MIFARE_1K::isDataBlock(block)) {
            getBlock(block, ptr);
            ptr += 0x10;
        }
    }
}

void MIFARE_1K::getRawData(uint8_t destination[0x400]) {
    memcpy(destination, data, 0x400);
}

/*
*******************************************************************************
*******************************************************************************
AUTHENTICATION*****************************************************************
*******************************************************************************
*******************************************************************************
*/


void MIFARE_1K::authenticate(PN532* pn532, uint8_t sector, bool keyA) {
    if (!isValidSector(sector)) throw CodedException(0x0B);
    pn532->MifareClassic_AuthenticateBlock(MIFARE_1K::sectorToBlock(sector) - 3, UID, keyA, keysA[sector]);
}


/*
*******************************************************************************
*******************************************************************************
READ/WRITE*********************************************************************
*******************************************************************************
*******************************************************************************
*/

void MIFARE_1K::updateSector(PN532* pn532, uint8_t sector, bool keyA) {
    if (!isValidSector(sector)) throw CodedException(0x0B);
	bool authenticated = false;
	
	for (uint8_t block = MIFARE_1K::sectorToBlock(sector)-3; block < MIFARE_1K::sectorToBlock(sector); block++) {
		if (altered[block]) {
			if (!authenticated) {
                authenticate(pn532, sector, keyA);
				authenticated = true;
			}
			pn532->MifareClassic_WriteBlock(block, data[block]);
            altered[block] = false;
		}
	}
}

void MIFARE_1K::update(PN532* pn532, bool keyA) {
	for (uint8_t sector = 0; sector < 0x10; sector++) {
        updateSector(pn532, sector, keyA);
	}
}

void MIFARE_1K::read(PN532* pn532, bool keyA) {
	for (uint8_t sector = 0; sector < 0x10; sector++) {
        readSector(pn532, sector, keyA);
	}
}

void MIFARE_1K::readSector(PN532* pn532, uint8_t sector, bool keyA) {
    if (!MIFARE_1K::isValidSector(sector)) throw CodedException(0x0B);
	
    authenticate(pn532, sector, keyA);
    
    uint8_t block = sectorToBlock(sector) - 3;
	for (; block <= MIFARE_1K::sectorToBlock(sector); block++) {
		pn532->MifareClassic_ReadBlock(block, data[block]);
	}
	
	getKeyA(sector, &data[block-1][0]); //Because the keys are hidden when read - since we know the key, we can copy it in
}

/*
*******************************************************************************
*******************************************************************************
SECURITY***********************************************************************
*******************************************************************************
*******************************************************************************
*/

void MIFARE_1K::changeKeyA(PN532* pn532, uint8_t sector, const uint8_t newKey[0x06]) {
    if (!MIFARE_1K::isValidSector(sector)) throw CodedException(0x0B);
	uint8_t newTrailer[0x10];
	uint8_t block = MIFARE_1K::sectorToBlock(sector);
    getBlock(block, newTrailer);
	memcpy(newTrailer, newKey, 0x06);
	
    authenticate(pn532, sector, true);
    pn532->MifareClassic_WriteBlock(block, newTrailer);
	
	memcpy(data[block], newTrailer, 0x10);
	setKeyA(sector, newKey);
}

void MIFARE_1K::changeKeyB(PN532* pn532, uint8_t sector, uint8_t const newKey[0x06]) {
    if (!MIFARE_1K::isValidSector(sector)) throw CodedException(0x0B);
    uint8_t newTrailer[0x10];
    uint8_t block = MIFARE_1K::sectorToBlock(sector);
    getBlock(block, newTrailer);
    memcpy(newTrailer + 0x0A, newKey, 0x06);
    
    authenticate(pn532, sector, false);
    pn532->MifareClassic_WriteBlock(block, newTrailer);
    
    memcpy(data[block], newTrailer, 0x10);
    setKeyB(sector, newKey);
}

void MIFARE_1K::changeAccessBits(PN532* pn532, uint8_t sector, const uint8_t newBits[0x03], bool keyA) {
    if (!MIFARE_1K::isValidSector(sector)) throw CodedException(0x0B);
	uint8_t newTrailer[0x10];
	uint8_t block = MIFARE_1K::sectorToBlock(sector);
    getBlock(block, newTrailer);
	memcpy(newTrailer + 0x06, newBits, 0x03);
	
    authenticate(pn532, sector, keyA);
    pn532->MifareClassic_WriteBlock(block, newTrailer);
    
	memcpy(data[block], newTrailer, 0x10);
	setAccessBits(sector, newBits);
}

/*
*******************************************************************************
*******************************************************************************
MAGIC FUNCTIONS****************************************************************
*******************************************************************************
*******************************************************************************
*/

void MIFARE_1K::changeBlockZero(PN532* pn532, const uint8_t newBlock[0x10], bool keyA) {
    if (!isMagic) throw CodedException(0x0C);
    setBlock(0x00, newBlock);
    updateSector(pn532, 0x00, keyA);
    memcpy(UID, &data[0][0], 4);
}

void MIFARE_1K::changeUID(PN532* pn532, const uint8_t newUID[0x04], bool keyA) {
    if (!isMagic) throw CodedException(0x0C);
    
    std::cout << "Printing old UID in case something goes wrong: " << std::endl;
    printHexBytes(data[0x00], 0x04, -1);
    
    uint8_t newBlock[0x10];
    getBlock(0x00, newBlock); //get old block zero
    memcpy(newBlock, newUID, 0x04); //put in new uid
    
    setBlock(0x00, newBlock); //write back
    calcBCC(); //and fix BCC
    
    updateSector(pn532, 0x00, keyA);
}

void MIFARE_1K::flagMagic() {
	isMagic = true;
}

void MIFARE_1K::clone(PN532* pn532, const uint8_t dataIn[0x40][0x10]) {
    //Loop over every block
    for (uint8_t block = 1; block < 0x40; block++) {
        if (MIFARE_1K::isDataBlock(block)) {
            //write the new data (not to physical card) and mark that block for update
            setBlock(block, dataIn[block]);
            flag(block);
        }
    }
    
    update(pn532, true); //Update with key A since this is the factory default best key
    
    //Change UID first - in case block zero will be read only
    
    changeBlockZero(pn532, dataIn[0x00], true);
    
    //This step must be done last, e.g. if we wanted to clone a card that was read only - write the data first and THEN set it to read only
    for (int sector = 0; sector < 0x10; sector++) {
        changeKeyB(pn532, sector, &data[sectorToBlock(sector)][0x0A]);
        changeKeyA(pn532, sector, data[sectorToBlock(sector)]);
        changeAccessBits(pn532, sector, &data[sectorToBlock(sector)][0x06], true);
    }
}

void MIFARE_1K::clone(PN532* pn532, const char* filename) {
    static uint8_t cloneData[0x40][0x10];
    readFile(filename, &cloneData[0][0], 0x400);
    return clone(pn532, cloneData);
}

/*
*******************************************************************************
*******************************************************************************
FILE I/O***********************************************************************
*******************************************************************************
*******************************************************************************
*/

void MIFARE_1K::makeFile(const char* filename) {
	writeFile(filename, &data[0][0], 0x400);
}

/*
*******************************************************************************
*******************************************************************************
MISC***************************************************************************
*******************************************************************************
*******************************************************************************
*/

void MIFARE_1K::paramsToData() {
	uint8_t block;
	for (uint8_t sector = 0; sector < 0x10; sector++) {
		block = MIFARE_1K::sectorToBlock(sector);
		getKeyA(sector, &data[block][0]);
		getAccessBits(sector, &data[block][0x06]);
		getSpareByte(sector, &data[block][0x09]);
		getKeyB(sector, &data[block][0x0A]);
	}
	memcpy(&data[0][0], UID, 0x04);
	calcBCC();
}

void MIFARE_1K::dataToParams() {
	uint8_t block;
	for (uint8_t sector = 0; sector < 0x10; sector++) {
		block = MIFARE_1K::sectorToBlock(sector);
		setKeyA(sector, &data[block][0]);
		setAccessBits(sector, &data[block][0x06]);
		setSpareByte(sector, &data[block][0x09]);
		setKeyB(sector, &data[block][0x0A]);
	}
	memcpy(UID, &data[0][0], 0x04);
}

void MIFARE_1K::dump() {
	printHexBytes(data[0], 0x400, 0x10);
}


void MIFARE_1K::flag(uint8_t block) {
    if (!MIFARE_1K::isDataBlock(block)) throw CodedException(0x0B);
	altered[block] = true;
}

/*
*******************************************************************************
*******************************************************************************
HELPERS************************************************************************
*******************************************************************************
*******************************************************************************
*/

void MIFARE_1K::calcBCC() {
	data[0][4] = data[0][0] ^ data[0][1] ^ data[0][2] ^ data[0][3];
}

bool MIFARE_1K::isValidSector(uint8_t sector) {
	return inRange(sector, (uint8_t)0x00, (uint8_t)0x0F);
}

bool MIFARE_1K::isValidBlock(uint8_t block) {
    return inRange(block, (uint8_t)0x00, (uint8_t)0x40);
}

bool MIFARE_1K::isTrailerBlock(uint8_t block) {
	return (block % 4 == 3);
}

bool MIFARE_1K::isDataBlock(uint8_t block) {
	return (inRange(block, (uint8_t)0x00, (uint8_t)0x3F) && !MIFARE_1K::isTrailerBlock(block));
}

bool isFirstBlock(uint8_t block) {
	return (block % 4 == 0);
}

uint8_t blockToSector(uint8_t block) {
	return (block - (block % 4))/4;
}

uint8_t MIFARE_1K::sectorToBlock(uint8_t sector) {
	return (sector * 4 + 3);
}

uint8_t byteToBlock(uint16_t byte) {
	return byte >> 4;
}

void MIFARE_1K::MIFAREInfo(PN532* pn532) {
	MIFARE_1K mifare(pn532);
	
	bool authenticated[0x10];
    memset(authenticated, true, 0x10);
	
	for (uint8_t sector = 0; sector < 0x10; sector++) {
		std::cout << "Attempting to authenticate sector 0x" << std::setw(2) << std::setfill('0') << std::hex << +sector << " with a default key: ";
        
        try {
            mifare.authenticate(pn532, sector, true);
        } catch (const PN532::PN532Exception& e){
            if (e.code == 0x14) {
                authenticated[sector] = false;
            } else throw; // Not authentication error - crash the program
        }
        
        if (authenticated[sector]) {
            std::cout << "success!";
        } else {
            std::cout << "failed!";
        }
		std::cout << std::endl << std::endl;
	}
	
	for (uint8_t sector = 0; sector < 0x10; sector++) {
		if (authenticated[sector]) {
			std::cout << "Reading sector 0x" << std::setw(2) << std::setfill('0') << std::hex << +sector << ":";
            mifare.readSector(pn532, sector, true);
			std::cout << std::endl;
		}
	}
	
	mifare.dump();
	
    //Check UID writable by writing the current UID back to itself
	if (authenticated[0x00]) {
		std::cout << "Testing if UID is writable: ";
		mifare.flagMagic();
		uint8_t dummyUID[4];
		mifare.getUID(dummyUID);
        
        //TODO CHECK ERROR CODE
        /*
        if (checkError(mifare.setUID(dummyUID))) {
			std::cout << "success! Is a magic card, UID writable" << std::endl;
		} else {
			std::cout << "failed! Is not a magic card, UID not writable" << std::endl;
		}
         */
	}
}

void trailerToConditions(uint32_t trailer) {
	uint8_t byte8 = trailer & 0xff;
	trailer >>= 0x08;
	uint8_t byte7 = trailer & 0xff;
	trailer >>= 0x08;
	uint8_t byte6 = trailer & 0xff;
	
	bool conditions[4][3];
	
	for (uint8_t block = 0; block < 4; block++) {
		conditions[block][0] = readBit(byte7, 4 + block);
		conditions[block][1] = readBit(byte8, block);
		conditions[block][2] = readBit(byte8, 4 + block);
		std::cout << "Block " << ": ";
		for (uint8_t n = 0; n < 3; n++) {
			if (conditions[block][n]) {
				std::cout << 1;
			} else {
				std::cout << 0;
			}
		}
		std::cout << std::endl;
	}
}

uint32_t conditionsToTrailer(uint8_t conditions[4]) {
	uint8_t byte6, byte7, byte8;
	byte6 = byte7 = byte8 = 0;
	
	bool boolConditions[4][3];
	
	for (uint8_t block = 0; block < 4; block++) {
		if (readBit(conditions[block], 0)) {
			setBit(&byte7, 4 + block);
		} else {
			setBit(&byte6, block);
		}
		
		if (readBit(conditions[block], 1)) {
			setBit(&byte8, block);
		} else {
			setBit(&byte6, 4 + block);
		}
		
		if (readBit(conditions[block], 2)) {
			setBit(&byte8, 4 + block);
		} else {
			setBit(&byte7, block);
		}
	}
	
	uint32_t trailer = byte6;
	trailer <<= 8;
	trailer ^= byte7;
	trailer <<= 8;
	trailer ^= byte8;
	
	return trailer;
}

