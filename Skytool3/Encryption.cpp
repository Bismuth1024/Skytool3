#include "Encryption.h"

CRC keyCRC(0x30, 0x42f0e1eba9ea3693, 0x9ae903260cc4); //The crc used to compute the key A s of a Skylander
CRC checkCRC(0x10, 0x1021, 0xffff); //The crc used to validate data (e.g. gold, xp)

bool Encryption::shouldEncryptBlock(uint8_t block) {
	return (MIFARE_1K::isDataBlock(block) && (inRange(block, (uint8_t)0x08, (uint8_t)0x15) || inRange(block, (uint8_t)0x24, (uint8_t)0x31)));
}

void Encryption::calcKeysA(Skylander* target) {
	for (uint8_t sector = 0; sector < 0x10; sector++) {
        calcKeyA(target, sector, target->keysA[sector]);
	}
}

void Encryption::calcKeyA(Skylander* target, uint8_t sector, uint8_t destination[6]) {
    if (!MIFARE_1K::isValidSector(sector)) throw CodedException(0x0B);

    if (sector == 0) {
        destination[0] = 0x4b;
        destination[1] = 0x0b;
        destination[2] = 0x20;
        destination[3] = 0x10;
        destination[4] = 0x7c;
        destination[5] = 0xcb;
        return;
    }

    uint8_t seed[5];
    target->getUID(seed);
    seed[4] = sector;

    keyCRC.compute(seed, 5, destination);
    swapEndian(destination, 6);
}

void Encryption::calcAESKey(Skylander* target, uint8_t block, uint8_t destination[0x10]) {
    if (!shouldEncryptBlock(block)) throw CodedException(0x0D);
	
	//Key is MD5 hash of a constant, first two blocks, and block number
	uint8_t md5seed[0x56] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x20, 0x43, 0x6F, 0x70, 0x79, 0x72, 0x69, 0x67, 0x68, 0x74, 0x20, 0x28, 0x43, 0x29, 0x20, // 0x00 "Copyright (C) "
        0x32, 0x30, 0x31, 0x30, 0x20, 0x41, 0x63, 0x74, 0x69, 0x76, 0x69, 0x73, 0x69, 0x6F, 0x6E, 0x2E, // "2010 Activision."
        0x20, 0x41, 0x6C, 0x6C, 0x20, 0x52, 0x69, 0x67, 0x68, 0x74, 0x73, 0x20, 0x52, 0x65, 0x73, 0x65, // " All Rights Rese"
        0x72, 0x76, 0x65, 0x64, 0x2E, 0x20}; // "rved. "

	memcpy(md5seed, target->data, 0x20);
	md5seed[0x20] = block;
    
    MD5 md5;
	md5.compute(destination, md5seed, 0x56);
}

bool Encryption::isEncrypted(Skylander* target) {
    return true;
}

void Encryption::decrypt(Skylander* target) {
    if (!isEncrypted(target)) throw CodedException(0x0E);
	for (uint8_t block = 0; block < 0x40; block++) {
		if (shouldEncryptBlock(block)) {
			decryptBlock(target, block);
		}
	}
}

void Encryption::encrypt(Skylander* target) {
    if (isEncrypted(target)) throw CodedException(0x0E);
	for (uint8_t block = 0; block < 0x40; block++) {
		if (shouldEncryptBlock(block)) {
			encryptBlock(target, block);
		}
	}
}

void Encryption::decryptBlock(Skylander* target, uint8_t block) {
    if (!shouldEncryptBlock(block)) throw CodedException(0x0D);
	
	AES aes(128);
	uint8_t key[16];
	calcAESKey(target, block, key);
	
	aes.DecryptECB(target->data[block], 16, key);
}

void Encryption::encryptBlock(Skylander* target, uint8_t block) {
    if (!shouldEncryptBlock(block)) throw CodedException(0x0D);

	AES aes(128);
	uint8_t key[16];
	calcAESKey(target, block, key);
	
	aes.EncryptECB(target->data[block], 16, key, 0);
}

void Encryption::validateChecksums(Skylander* target) {
    if (isEncrypted(target)) throw CodedException(0x0F);
	uint8_t newChecksum[0x02];
	uint8_t oldChecksum[0x02];
	
    std::cout << "Type 0 checksum:" << std::endl;
    std::cout << "\tCalculated checksum:\n\t";
	checksum(target, 0, 0, newChecksum);
	printHexBytes(newChecksum, 2);
	
    std::cout << "\tOld checksum:\n\t";
	target->getBytes(Locations::crc[0], 0, oldChecksum);
	printHexBytes(oldChecksum, 2);
	
    if (memcmp(newChecksum, oldChecksum, 0x02) != 0) throw CodedException(0x10);
			
	for (uint8_t area = 1; area <= 2; area++) {
		for (uint8_t type = 4; type >= 1; type--) {
            std::cout << std::dec << std::setw(1) << "Type " << +type << " checksum area " << +area << ":" << std::endl;
			
            std::cout << "\tCalculated checksum:\n\t" << std::endl;
			checksum(target, type, area, newChecksum);
			printHexBytes(newChecksum, 2);
			
            std::cout << "\tOld checksum:\n\t" << std::endl;
			target->getBytes(Locations::crc[type], area, oldChecksum);
			printHexBytes(oldChecksum, 2);
			
            if (memcmp(newChecksum, oldChecksum, 0x02) != 0) throw CodedException(0x10);
		}
	}
    
    std::cout << "All checksums validated!" << std::endl;
}

void Encryption::updateChecksums(Skylander* target) {
    if (isEncrypted(target)) throw CodedException(0x0F);

	uint8_t temp[2];
	
	for (uint8_t area = 1; area <= 2; area++) {
		for (uint8_t type = 4; type >= 1; type--) {
			checksum(target, type, area, temp);
			target->setBytes(Locations::crc[type], area, temp);
		}
	}
}

void Encryption::checksum(Skylander* target, uint8_t type, uint8_t area, uint8_t destination[0x02]) {
    if (isEncrypted(target)) throw CodedException(0x0F);

	uint16_t nBytes;
	uint8_t src[0x110];
	
	uint8_t headerBlock = target->areaBlock(area);
	
	switch (type) {
		case 0:
			nBytes = 0x1E;
			memcpy(src, target->data, nBytes);
			break;
			
		case 1:
			nBytes = 0x10;
			memcpy(src, target->data[headerBlock], nBytes);
			src[0x0E] = 0x05;
			src[0x0F] = 0x00;
			break;
			
		case 2:
			nBytes = 0x30;
			memcpy(src, target->data[headerBlock + 1], 0x20);
			memcpy(src + 0x20, target->data[headerBlock + 4], 0x10);
			break;
			
		case 3:
			nBytes = 0x110;
			memcpy(src, target->data[headerBlock + 5], 0x20);
			memcpy(src + 0x20, target->data[headerBlock + 8], 0x10);
			memset(src + 0x30, 0x00, 0xE0);
			break;
			
		case 4:
			nBytes = 0x40;
			memcpy(src, target->data[headerBlock + 0x09], 0x20);
			memcpy(src + 0x20, target->data[headerBlock + 0x0C], 0x20);
			src[0x00] = 0x06;
			src[0x01] = 0x01;
			break;
			
		default:
            throw CodedException(0x11);
	}
	
	checkCRC.compute(src, nBytes, destination);
	swapEndian(destination, 2);
}

