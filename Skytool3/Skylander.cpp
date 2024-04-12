#include "Skylander.h"

uint8_t zeroBlock[0x10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint8_t sectorZero[11] = {0x81, 0x01, 0x0f, 0xc4, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15};

uint8_t trailerBytes[0x10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x0f, 0x08, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t zeroTrailer[0x10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x0f, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


uint32_t minXPforLevel[22] = {
								0,
								0, //1
								1000,
								2200,
								3800,
								6000, //5
								9000,
								13000,
								18200,
								24800,
								33000, //10
								42700,
								53900,
								66600,
								80800,
								96500, //15
								113700,
								132400,
								152600,
								174300,
								197500, //20
                                0xFFFFFFFF
								};

/*
*******************************************************************************
*******************************************************************************
CONSTRUCTORS*******************************************************************
*******************************************************************************
*******************************************************************************
*/

Skylander::Skylander() : MIFARE_1K() {}

Skylander::Skylander(const char* filename) : MIFARE_1K(filename) {
	dataToParams();
}

Skylander::Skylander(PN532* _nfc) : MIFARE_1K(_nfc) {
	Encryption::calcKeysA(this);
}

Skylander::Skylander(PN532* nfc, const char* charName, uint16_t _typeCode) : MIFARE_1K(nfc) {
	uint8_t key[6];
	for (uint8_t sector = 0x00; sector < 0x10; sector++) {
		Encryption::calcKeyA(this, sector, key);
		changeKeyA(nfc, sector, key);
	}
	uint8_t newBlockZero[16];
	memcpy(newBlockZero + 0x05, sectorZero, 0x0b);
	memcpy(newBlockZero, &data[0][0], 0x05);
	flagMagic();
    changeBlockZero(nfc, newBlockZero, true);
	
	uint16_t _charCode = convertCharName(charName);
                    if (_charCode == 0xFFFF) ;
	setCharacter(_charCode, _typeCode);
    update(nfc, true);
}


/*
*******************************************************************************
*******************************************************************************
GET/SET************************************************************************
*******************************************************************************
*******************************************************************************
*/

uint16_t Skylander::getCharCode() {
	return getValue(Locations::charCode, 0);
}

uint16_t Skylander::getTypeCode() {
	return getValue(Locations::typeCode, 0);
}

void Skylander::setCharacter(uint16_t _charCode, uint16_t _typeCode) {
	setValue(Locations::charCode, saveArea, _charCode);
	setValue(Locations::typeCode, saveArea, _typeCode);
	Encryption::checksum(this, 0x00, 0x00, &data[0x01][0x0E]);
}

uint16_t Skylander::getGold() {
    getArea();
	return getValue(Locations::gold, saveArea);
}

void Skylander::setGold(uint16_t Gold) {
	getArea();
	return setValue(Locations::gold, saveArea, Gold);
}

uint32_t Skylander::getXP() {
    getArea();
    uint32_t out = 0;
	for (uint8_t i = 0; i < 3; i++) {
		out += getValue(Locations::xp[i], saveArea);
	}
	return out;
}

void Skylander::setXP(uint32_t XP) {
	getArea();
	if (XP <= 33000) {
		setValue(Locations::xp[0], saveArea, XP);
	} else {
		setValue(Locations::xp[0], saveArea, 33000);
		XP -= 33000;
		if (XP <= 63500) {
			setValue(Locations::xp[1], saveArea, XP);
		} else {
			setValue(Locations::xp[1], saveArea, 63500);
			XP -= 63500;
            if (XP > 0xfffffff) throw CodedException(0x13);
			setValue(Locations::xp[2], saveArea, XP);
		}
	}
}

uint8_t Skylander::getLevel() {
    getArea();
	uint32_t XP = getXP();
	return XPtoLevel(XP);
}

void Skylander::setLevel(uint8_t level) {
	getArea();
	uint32_t XP = levelToXP(level);
	return setXP(XP);
}

uint32_t Skylander::getHeroics(uint8_t game) {
    getArea();
	return getValue(Locations::heroics[game], saveArea);
}

void Skylander::setHeroics(uint32_t h, uint8_t game) {
	getArea();
	return setValue(Locations::heroics[game], saveArea, h);
}

uint16_t Skylander::getPlaytime() {
    getArea();
	return getValue(Locations::playtime, saveArea);
}

void Skylander::setPlaytime(uint16_t p) {
	getArea();
	return setValue(Locations::playtime, saveArea, p);
}

void Skylander::getFirstPlayed(uint8_t destination[0x06]) {
	getArea();
	return getBytes(Locations::history[1], saveArea, destination);
}

void Skylander::getLastPlayed(uint8_t destination[0x06]) {
	getArea();
	return getBytes(Locations::history[0], saveArea, destination);
}

std::string Skylander::getHat() {
    getArea();
    uint16_t code = 0;
    int game = 0;
    for (; game <= 4; game++) {
        code = getValue(Locations::hats[game], saveArea);
        if (code > 0) break;
    }
    if (game == 4) code += 0xFF;
    return getHatName(code);
}

void Skylander::setHat(std::string hatName) {
    getArea();
    uint16_t code = getHatCode(hatName);
    if (code == 0xFFFF) throw CodedException(0x14);
    uint8_t game = getHatGame(code);
    if (game == 4) code -= 0xFF;
    
    for (uint8_t i = 0; i <= 4; i++) {
        setValue(Locations::hats[i], saveArea, 0);
    }
    setValue(Locations::hats[game], saveArea, code);
}


bool Skylander::getName() {
	//Reminder that this is a bool so it can be used to test if encrypted
	getArea();
	uint8_t block = areaBlock(saveArea);
	
	Name.clear();
	uint16_t nextChar;
	
	for (uint8_t i = 0; i < (Locations::name[0]).size; i += 2) {
		nextChar = bytesToInt(data[block + Locations::name[0].block] + i, 2, true);
		if (nextChar > 0x007f) return false;
		Name += (char)nextChar;
		if (nextChar == 0) return true;
	}
	
	for (uint8_t i = 0; i < Locations::name[1].size; i += 2) {
		nextChar = bytesToInt(data[block + Locations::name[1].block] + i, 2, true);
		if (nextChar > 0x7f) return false;
		Name += (char)nextChar;
		if (nextChar == 0) return true;
	}
	return true;
}

void Skylander::setName(std::string newName) {
	getArea();
	uint8_t block = Locations::name[0].block + areaBlock(saveArea);
	uint8_t offset = 0;
	int len = newName.length();
    if (len > 15) throw CodedException(0x12);
		
	setBytes(Locations::name[0], saveArea, zeroBlock);
	setBytes(Locations::name[1], saveArea, zeroBlock);
	
	char nextChar;
	for (uint8_t i = 0; i < newName.length(); i++) {
		nextChar = newName[i];
		data[block][offset++] = nextChar;
		data[block][offset++] = 0x00;
		if (offset == 0x10) {
			offset = 0;
			block += 2;
		}
		
	}
	
	intToBytes((uint16_t)0x0000, 2, &data[block][offset], true);
}

/*
*******************************************************************************
*******************************************************************************
AREAS**************************************************************************
*******************************************************************************
*******************************************************************************
*/

int Skylander::getArea() {
	if (getValue(Locations::save, 1) > getValue(Locations::save, 2)) saveArea = 1;
	if (getValue(Locations::save, 2) > getValue(Locations::save, 1)) saveArea = 2;
	return 0;
}

uint8_t Skylander::areaBlock(uint8_t area) {
	if (area == 1) return 0x08;
	if (area == 2) return 0x24;
	return 0x00;
}





















void Skylander::superchargerFormat(PN532* nfc) {
	uint8_t newTrailer[0x10];
	
	for (uint8_t sector = 0; sector < 0x10; sector++) {
		uint8_t block = sectorToBlock(sector);
		if (sector == 0) {
			memcpy(newTrailer, zeroTrailer, 0x10);
		} else {
			memcpy(newTrailer, trailerBytes, 0x10);
		}
		
		getKeyA(sector, newTrailer);
        

        nfc->MifareClassic_WriteBlock(block, newTrailer);
	}
}

void Skylander::loadBackup(const char* filename) {
	uint8_t temp[0x04];
	readFile(filename, temp, 0x04);
	
    if (memcmp(temp, UID, 0x04) != 0) throw CodedException(0x15);
	
	loadFromFile(filename, 0x04, 0x3F);
}

void Skylander::printInfo() {
    
	uint16_t charCode = getCharCode();
	std::string charName = getCharName(charCode);
	uint16_t typeCode = getTypeCode();
	
    std::cout << "Character code: " << std::hex << std::setw(4) << std::setfill('0') << +charCode << std::endl;
	std::cout << "Character: " << charName << std::endl;
    std::cout << "Type code: " << std::hex << std::setw(4) << std::setfill('0') << +typeCode << std::endl;

	//The rest of this is garbage if not encrypted - one disadvantage of decoupling encryption and skylander

	getArea();
	
	int heroics = countSetBits(getHeroics(0)) + countSetBits(getHeroics(1));
	uint16_t gold = getGold();
	uint32_t xp = getXP();
	uint8_t level = XPtoLevel(xp);
    std::string hatName = getHat();
	uint16_t playtime = getPlaytime();
    uint16_t seconds = playtime % 60;
    uint16_t minutes = playtime / 60;
		
	getName();
	
	uint8_t firstPlayed[6], lastPlayed[6];
	
	getFirstPlayed(firstPlayed);
	getLastPlayed(lastPlayed);
	
	uint16_t yearFirst = bytesToInt(firstPlayed + 0x04, 0x02, true);
	uint16_t yearLast = bytesToInt(lastPlayed + 0x04, 0x02, true);


    std::cout << "Gold: " << std::dec << gold << std::endl;
    std::cout << "XP: " << xp << ", level: " << level << std::endl;
    std::cout << "Playtime: " << minutes << " minutes, " << seconds << " seconds" << std::endl;
    std::cout << "Heroics: " << heroics << std::endl;
    
    std::cout << "Last Played: " << std::dec << std::setw(2) << std::setfill('0') << +lastPlayed[2] << "/" << +lastPlayed[3] << "/" << std::setw(4) << yearLast << " " << +lastPlayed[1] << ":" << +lastPlayed[0] << std::endl;

    std::cout << "Hat: " << hatName << std::endl;
	std::cout << "Name: " << Name << std::endl;
}

uint8_t Skylander::XPtoLevel(uint32_t XP) {
	for (uint8_t level = 1; level < 50; level++) {
		if (XP < minXPforLevel[level]) return (level - 1);
	}
	return 0;
}

uint32_t Skylander::levelToXP(uint8_t level) {
	return (minXPforLevel[level]);
}


uint64_t Skylander::getValue(Locations::dataInfo location, uint8_t area) {
    if (area > 2) throw CodedException(0x16);
	uint64_t out = 0;
	
	uint8_t* startptr = &data[areaBlock(area) + location.block][location.offset];
	uint8_t len = location.size;
	
	for (uint8_t i = 0; i < len; i++) {
		out = out << 8;
		out |= startptr[len - i - 1];
	}

	return out;
}

void Skylander::setValue(Locations::dataInfo location, uint8_t area, uint64_t val) {
    if (area > 2) throw CodedException(0x16);

	uint8_t nBytes = location.size;
	uint8_t block = areaBlock(area) + location.block;
	uint8_t* destination = &data[block][location.offset];
	
	flag(block);
    intToBytes(val, nBytes, destination, true);
}

void Skylander::getBytes(Locations::dataInfo location, uint8_t area, uint8_t* destination) {
    if (area > 2) throw CodedException(0x16);

    uint8_t block = areaBlock(area) + location.block;
    uint8_t len = location.size;
    uint8_t offset = location.offset;
    
    return MIFARE_1K::getBytes(block, offset, destination, len);
}

void Skylander::setBytes(Locations::dataInfo location, uint8_t area, uint8_t* dataIn) {
    if (area > 2) throw CodedException(0x16);

	uint8_t block = areaBlock(area) + location.block;
	uint8_t len = location.size;
	uint8_t offset = location.offset;
	
    MIFARE_1K::setBytes(block, offset, dataIn, len);
    flag(block);
}



