#ifndef LOCATIONS_H_GUARD_
#define LOCATIONS_H_GUARD_

/*
 A list of all the locations of important data values on the skylander's mifare chip.
 
 Note that the 'block' field does not refer to block number of mifare 1k, see below description
 
 DATA LAYOUT 
 
 There are two duplicate data areas - 0x08-0x15, and 0x24-0x31.  Each of them have data stored at the block, offset, and length specified below
 
 There is also some data without a copy, in the first sector of the MIFARE chip
 
 Each data area is also encrypted, see encryption.h
 
 All little endian on the MIFARE chip.
 
 */

#include "MIFARE_1K.h"
#include <stdint.h>


namespace Locations {
	struct dataInfo {
		uint8_t block;
		uint8_t offset;
		uint8_t size;
	};
	
	const dataInfo charCode = {0x01, 0x00, 0x02}; //The code to identify the character, e.g. trigger happy
	const dataInfo typeCode = {0x01, 0x0C, 0x02}; //Identifies the variant e.g. series 2

	const dataInfo crc[5] = { // 5 different CRCs for validating data
					{0x01, 0x0E, 0x02}, //Type 0 for only sector zero, only one copy
					{0x00, 0x0E, 0x02}, // Other types 1-4 for the duplicated areas, see Encryption
					{0x00, 0x0C, 0x02},
					{0x00, 0x0A, 0x02},
					{0x09, 0x00, 0x02}	
				};	

	const dataInfo xp[3] = {
					{0x00, 0x00, 0x03}, //XP in 3 zones - first zone stores up to 33000, next up to 63500 (total), and next the rest
					{0x09, 0x03, 0x02},
					{0x09, 0x08, 0x03}
				};	

	const dataInfo heroics[2] = {
					{0x05, 0x06, 0x04}, //Spyro's
					{0x0A, 0x04, 0x03} //Giants? i think need to check
				};

	const dataInfo quests[2] = {
					{0x0A, 0x07, 0x09}, //Again need to check, game 3 i think?
					{0x0C, 0x07, 0x09}
				};


	const dataInfo gold = {0x00, 0x03, 0x02};

	const dataInfo playtime = {0x00, 0x05, 0x02}; //In min?

	const dataInfo save = {0x00, 0x09, 0x01}; //Whichever save area has the higher value of this number is the most recently used area

	const dataInfo upgrades = {0x01, 0x00, 0x02}; //Bit flags for upgrades

	const dataInfo platforms = {0x01, 0x03, 0x01}; //What platforms the skylander has been to? bit flags e.g. playstation, xbox

	const dataInfo hats[5] = { //Each hat for each game is stored in a different location, that way you can move between games without losing hats
					{0x01, 0x04, 0x01},
					{0x09, 0x05, 0x01},
					{0x09, 0x0C, 0x01},
                    {0x09, 0x0C, 0x01},
					{0x09, 0x0E, 0x01}
				};	

	const dataInfo ownership = {0x01, 0x08, 0x08}; //Some value unique to a physical console to take ownership of skylander

	const dataInfo name[2] = { //Unicode, split in half
					{0x02, 0x00, 0x10},
					{0x04, 0x00, 0x10},
				};	
				
	const dataInfo history[2] = { //Last used and first used timestamp
					{0x05, 0x00, 0x06},
					{0x06, 0x00, 0x06},
				};
}

#endif









