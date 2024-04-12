#ifndef ENCRYPTION_H_GUARD_
#define ENCRYPTION_H_GUARD_

/*
 A class used to manage the encryption/checksums of a skylander.  Originally this was in the skylander file but i chose to separate the classes
 to not make the skylander class too big
 
 MIFARE encryption
    -Key A is used to read skylanders, the sector zero key A is always 4b 0b 20 10 7c cb
    -Every other key A is a CRC on the UID concatenated with the sector number, see cpp file for specifics
 
 AES encryption
    -All of the used data blocks are encrypted via AES ECB.
    -The key is the MD5 hash of a concatenation of a constant, first two blocks, and the block number (i.e. one key per block)
 
 CRCs
    -There are 5 different CRCs stored in the chip.  One has only one copy and is used to validate block zero
    -the other 4 have one per data area (see Locations) and are used to validate various parts of the data
    -Note that types 1-4 should be done in reverse order, because results of higher types are part of the input for lower types
 
 The responsibility is up to the user of this code to encrypt/decrypt the skylander.  
 */

#include <stdint.h>
#include "Skylander.h"
#include <iostream>
#include "MD5.h"
#include "misc.h"
#include "CRC.h"

class Skylander;

class Encryption {
	public:
		static void encrypt(Skylander* target); //Encrypts a Skylander
		static void decrypt (Skylander* target); //Decrypts a Skylander
		static void validateChecksums(Skylander* target); //Checks all checksums vs what they are supposed to be
		static void updateChecksums(Skylander* target); //Recalculates all checksums
    static void calcKeysA(Skylander* target);


	private:
		friend class Skylander;
		static bool shouldEncryptBlock(uint8_t block); //Whether a block needs to be encrypted/decrypted
    
        /*
         checksum: calculates a specific checksum type for a specific data area (0 or 1)
         */
		static void checksum(Skylander* target, uint8_t type, uint8_t area, uint8_t destination[0x02]);

	
		static void calcAESKey(Skylander* target, uint8_t block, uint8_t destination[0x10]);
		static void decryptBlock(Skylander* target, uint8_t block);
		static void encryptBlock(Skylander* target, uint8_t block);

		static bool isEncrypted(Skylander* target);
	
		static void calcKeyA(Skylander* target, uint8_t sector, uint8_t destination[0x10]);

};

#endif
