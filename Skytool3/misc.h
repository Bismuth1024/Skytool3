/*
 A bunch of miscellaneous helper functions.
 
 Mainly stuff like printing numbers in a certain format for debugging, or converting endianness, bit setting, etc
 
 
 
 */
#ifndef MISC_H_GUARD_
#define MISC_H_GUARD_

#define LP (
#define RP (

#include <filesystem>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdarg.h>
#include "exceptions.h"

/*
 swapEndian: swaps the endianness of an array of bytes (that represent a number) by swapping the byte order
 
 data: pointer to the beginning of the byte array
 nBytes: number of bytes
 
 */
void swapEndian(uint8_t* data, int nBytes);


/*
 printHexBytes: prints raw byte data as human readable bytes, optionally separating large data into rows.
 
 data: pointer to beginning of byte array
 nBytes: number of bytes
 nColumns: if -1, will just print until the end in one line.  If positive, each line will contain nColumns bytes.
 */

void printHexBytes(uint8_t* data, int nBytes, int nColumns);

/*
 inRange: tests whether a numeric value is between upper and lower bounds (inclusive)
 
 x: value to test
 lower: lower bound
 upper: upper bound
 
 return value: whether the value is in the range
 */
template <typename T>
bool inRange(T x, T lower, T upper);

/*
 bytesToInt: converts a byte array to an unsigned integer little or big endian.  Will throw an exception if
 len is too big for the integer to store.
 
 data: pointer to the start of the byte array
 len: number of bytes to convert
 littleEndian: true for little, false for big
 
 ****maybe check this - template prone to bugs? no check for unsigned int
 
 */
template <typename T>
T bytesToInt(uint8_t* data, uint8_t nBytes, bool littleEndian);

/*
 intToBytes: converts an integer to byte array little or big endian.  Will throw an exception if
 len is too big for the byte array to store.
 
 num: number to convert
 nBytes: number of bytes to convert
 littleEndian: true for little, false for big
 
 return value: the decoded integer
 
 ****maybe check this - template prone to bugs? no check for unsigned int
 
 */
template <typename T>
void intToBytes(T num, uint8_t nBytes, uint8_t* destination, bool littleEndian);



/*
 compareBytes: compares two byte arrays (of equal size) and prints out where they differ.
 
 array1: pointer to array 1
 array2: pointer to array 2
 nBytes: bytes to compare
 nColumns: see printHexBytes
 
 */

void compareBytes(uint8_t* array1, uint8_t* array2, int nBytes, int nColumns);

/*
 readFile: reads a binary data file into a byte array
 
 filename: Name of the file
 destination: destination for the file data
 nBytes: Number of bytes to read
 
 ****should probably add checks for file size, same for writeFile
 */
void readFile(const char* filename, uint8_t* destination, int nBytes);

/*
 writeFile: reads a binary data file into a byte array
 
 filename: Name of the file
 data: source for the file data
 nBytes: Number of bytes to write
 
 */
void writeFile(const char* filename, uint8_t* data, int nBytes);



int countSetBits(uint64_t x);
uint64_t setBits(int n);

bool inline readBit(uint8_t x, uint8_t n) {
	return ((x >> n) & 0x01);
}

void inline setBit(uint8_t* x, uint8_t n) {
	*x |= (0x01 << n);
}

void inline clearBit(uint8_t* x, uint8_t n) {
	*x &= ~(0x01 << n);
}
void inline littleEndian(int x, uint8_t nBytes, uint8_t* destination) {
	memcpy(destination, &x, nBytes);
}



#endif
