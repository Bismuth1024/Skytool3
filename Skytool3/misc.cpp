#include "misc.h"

void swapEndian(uint8_t* data, int nBytes) {
	uint8_t temp;
    
    uint8_t *left = data;
    uint8_t *right = data + nBytes - 1;
    
    while (left < right) {
        temp = *left;
        *left = *right;
        *right = temp;
        left++;
        right--;
    }
}


void printHexBytes(uint8_t* data, int nBytes, int nColumns) {
    if (nColumns < 0 || nBytes < nColumns) {//Single line print for negative nColumns input
        for (int i = 0; i < nBytes; i++) {
            std::cout << std::setw(2) << std::setfill('0') << std::hex << +data[i] << " "; //Print each byte
        }
        std::cout << std::endl; //And a newline
        return;
    }
    
    std::cout << "Printing in blocks of " << std::dec << nColumns << ":" << std::endl; //Display row length
    
    int block = 0;
    for (int i = 0; i < nBytes; i++) {
        if ((i % nColumns) == 0) {
            std::cout << std::endl;
            std::cout << "Block " << block << ": \t"; //Label the start of each block (row)
            block++;
        }
        std::cout << std::setw(2) << std::setfill('0') << std::hex << +data[i] << " "; //Print each byte
    }
    std::cout << std::endl; //And a newline
}

template <typename T>
bool inRange(T x, T lower, T upper) {
	return (x >= lower && x <= upper);
}

uint64_t bytesToInt(uint8_t* data, uint8_t nBytes, bool littleEndian) {
    
    if (nBytes > 8) throw CodedException(0x01);
    
	uint64_t out = 0;

	for (uint8_t i = 0; i < nBytes; i++) {
		out = out << 8;
        if (littleEndian) {
            out |= data[nBytes - i - 1];
        } else {
            out |= data[i];
        }
	}

	return out;
}

template <typename T>
void intToBytes(T num, uint8_t nBytes, uint8_t* destination, bool littleEndian) {
	
	uint64_t threshold = (0x01ULL << (nBytes << 3));
	
    if (num >= threshold) throw CodedException(0x01);
	
	for (uint8_t i = 0; i < nBytes; i++) {
        if (littleEndian) {
            destination[i] = num & 0xff;
        } else {
            destination[nBytes - i - 1] = num & 0xff;
        }
        num = num >> 8;
	}
	
}


void compareBytes(uint8_t* array1, uint8_t* array2, int nBytes, int nColumns) {
    std::cout << "Data 1: " << std::endl;
    printHexBytes(array1, nBytes, nColumns);
    
    std::cout << "Data 2: " << std::endl;
    printHexBytes(array2, nBytes, nColumns);
    
    uint8_t* difference = new uint8_t[nBytes];
    memset(difference, 0x00, nBytes);
    
    int flag = -1; //This will denote the start of a differing section of bytes.
    
    for (int i = 0; i < nBytes; i++) {
        if (array1[i] != array2[i]) {//If mismatch
            if (flag == -1) {
                flag = i; //Record as the start of a mismatched section
            }
            difference[i] = 0xff;
        } else {
            if (flag != -1) { //This is then the end of a mismatched section
                if (i == flag + 1) {

                    std::cout << "Data differs at byte " << std::setw(4) << std::setfill('0') << std::hex << +flag << ", array 1 contains " << std::setw(2) << +array1[flag] << " and array 2 contains " << +array2[flag] << std::endl;
                } else {
                    std::cout << "Data differs at bytes " << std::setw(4) << std::setfill('0') << std::hex << +flag << " to " << (i - 1) << ", array 1 contains: " << std::endl;
                    printHexBytes(array1 + flag, i - flag, nColumns);
                    
                    std::cout << "Whereas array 2 contains: " << std::endl;
                    printHexBytes(array2 + flag, i - flag, nColumns);
                }
                flag = -1;
            }
        }
    }
    
    std::cout << "Overview of differences: " << std::endl;
    printHexBytes(difference, nBytes, nColumns);
    
    delete [] difference;
}


void readFile(const char* filename, uint8_t* destination, int nBytes) {
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	
    if (!file) throw CodedException(0x02);
    	
	file.read(reinterpret_cast<char *>(destination), nBytes);
	file.close();
}

void writeFile(const char* filename, uint8_t* data, int nBytes) {
	std::ofstream file(filename, std::ios::out | std::ios::binary);
    
    if (!file) throw CodedException(0x02);

	file.write(reinterpret_cast<char *>(data), nBytes);
	file.close();
}


int countSetBits(uint64_t x) {
    uint8_t count = 0;
    while (x) {
        x &= (x - 1);
        count++;
    }
    return count;
}


uint64_t setBits(int n) {
    return ((uint64_t)(1ll << n) - 1);
}

template bool inRange(uint8_t, uint8_t, uint8_t);
template void intToBytes(uint64_t, uint8_t, uint8_t*, bool);
template void intToBytes(uint16_t, uint8_t, uint8_t*, bool);




