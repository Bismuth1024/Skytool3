#include "CRC.h"

CRC::CRC(uint8_t width, uint64_t polynomial, uint64_t initial) :
width(width), polynomial(polynomial), initial(initial) {}

/*

Description: Computes the CRC of a byte array, outputting the CRC as a byte array.

Arguments:	input - Pointer to byte array of input.
			nBytes - Number of bytes in input.
			destination - Pointer to byte array destination of CRC

Returns: None

*/

void CRC::compute(uint8_t* input, int nBytes, uint8_t* destination) {
    uint64_t trim = 0xffffffffffffffff >> (0x40 - width); //Trim to correct width
    uint64_t msbcheck = 0x8000000000000000 >> (0x40 - width); //And check at the right position
    uint64_t crc = initial;  //Initialise register

    for (int i = 0; i < nBytes; i++) {
        uint64_t byte64 = input[i]; //Expand size of byte to allow left shifts to work properly
        crc = crc ^ (byte64 << (width - 8)); //Extended to required length
        for (uint8_t k = 0; k < 8; k++) { //Do for each bit
            if (crc & msbcheck) { //If first bit is 1 do the XOR
                crc = (crc << 1) ^ polynomial;
            }
            else {
                crc = crc << 1;
            }
            crc = crc & trim; //Keep CRC in required length
        }
    }

    uint8_t bytesOut = width >> 3;

    #ifdef LITTLE_ENDIAN
        intToBytes(crc, bytesOut, destination, false);
    #else
        intToBytes(crc, bytesOut, destination, true);
    #endif

    return;
}
