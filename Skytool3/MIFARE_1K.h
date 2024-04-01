#ifndef MIFARE_HEADER_GUARD
#define MIFARE_HEADER_GUARD

#include "misc.h"
#include "PN532.h"
#include <memory.h>
#include <stdio.h>
#include <stdint.h>
#include <fstream>

class PN532;


/*
 A class for representing a MIFARE 1K NFC chip.  Since there is the representation of the chip's data in computer memory which
 may or may not differ from the actual data on the chip, the following model is used.
 
 All data is stored in computer memory
 
 Make changes to the data blocks using setBytes or setBlock.  When ready to update, call updateSectorA/B or updateA/B
 
 every data change marks that block as edited, and it will be updated to the physical card when calling updateA/B etc
 
 to chaange sector trailers (key and access bits) use the dedicated changeKeyA etc. functions, these are immediately written to the card.
 the setKeyA setKeyB functions DO NOT actually change the physical card's key - they just set the key to use for authenticaation.
 
 For example you might have a card where you know the key A for sector 3 is 0x112233445566, so you first call setKeyA(0x112233445566)
 and then you can authenticateA(3) before read and writes.
 
 
 
 
 
 
 
 
 */


class MIFARE_1K {
public:
    
    /*
     The default values for a MIFARE 1K chip
     */
    static constexpr uint8_t defaultZero[0x0B] = {0x08, 0x04, 0x00, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69};
    static constexpr uint8_t defaultKey[0x06] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    static constexpr uint8_t defaultAccessBits[0x03] = {0xFF, 0x07, 0x80};
    static constexpr uint8_t defaultSpareByte = 0x69;

    //Constructors
    MIFARE_1K();
    MIFARE_1K(const char* filename);
    MIFARE_1K(const uint8_t dataIn[0x40][0x10]);
    MIFARE_1K(PN532* pn532);
    
    
    //Get/set
    /*
     getKeyA
     
     Copies from a sector's keyA variable into a given byte array.
     
     sector: sector to get the key of
     destination: destination to copy to
     */
    void getKeyA(uint8_t sector, uint8_t destination[0x06]);
    
    /*
     setKeyA
     
     Copies from a given byte array into a sector's keyA variable (DOES NOT CHANGE THE PHYSICAL CARD's KEY)
     
     sector: sector to set the key of
     key: key to copy from
     */
    void setKeyA(uint8_t sector, const uint8_t key[0x06]);
    
    /*
     getKeyB
     
     Copies from a sector's keyB variable into a given byte array.
     
     sector: sector to get the key of
     destination: destination to copy to
     */
    void getKeyB(uint8_t sector, uint8_t destination[0x06]);
    
    /*
     setKeyB
     
     Copies from a given byte array into a sector's keyA variable (DOES NOT CHANGE THE PHYSICAL CARD's KEY)
     
     sector: sector to set the key of
     key: key to copy from
     */
    void setKeyB(uint8_t sector, const uint8_t key[0x06]);
    
    /*
     getAccessBits
     
     Copies from a sector's accessBits variable into a given byte array.
     
     sector: sector to get the access bits of
     destination: destination to copy to
     */
    void getAccessBits(uint8_t sector, uint8_t destination[0x03]);
    
    /*
     setAccessBits
     
     Copies from a given byte array into a sector's accessBits variable (DOES NOT CHANGE THE PHYSICAL CARD's BITS)
     
     sector: sector to set the key of
     bits: access bits to copy from
     */
    void setAccessBits(uint8_t sector, const uint8_t bits[0x03]);
    
    /*
     getSpareByte
     
     Copies from a sector's spareByte variable into a given byte pointer. (byte 0x09 of sector trailer)
     
     sector: sector to get the byte of
     destination: destination to copy to
     */
    void getSpareByte(uint8_t sector, uint8_t* destination);
    
    /*
     setSpareByte
     
     Copies from a byte pointer into a sector's spareByte variable (byte 0x09 of sector trailer)
     
     sector: sector to get the byte of
     newByte: source for the new byte
     */
    void setSpareByte(uint8_t sector, const uint8_t* newByte);
    
    void getBytes(uint8_t block, uint8_t offset, uint8_t* destination, uint8_t len);
    void setBytes(uint8_t block, uint8_t offset, const uint8_t* dataIn, uint8_t len);
    
    void getBlock(uint8_t block, uint8_t destination[0x10]);
    void setBlock(uint8_t block, const uint8_t* blockIn);
    
    
    void getUID(uint8_t destination[0x04]);
    void setUID(uint8_t const newUID[0x04]);

    void getData(uint8_t destination[0x300]);
    void getRawData(uint8_t destination[0x400]);
    
    
    //Authentication
    void authenticate(PN532* pn532, uint8_t sector, bool keyA);

    //Read/write
    void read(PN532* pn532, bool keyA);
    
    void readSector(PN532* pn532, uint8_t sector, bool keyA);
    
    void update(PN532* pn532, bool keyA);

    void updateSector(PN532* pn532, uint8_t sector, bool keyA);
    
    
    //Security
    void changeKeyA(PN532* pn532, uint8_t sector, uint8_t const newKey[0x06]);
    void changeKeyB(PN532* pn532, uint8_t sector, uint8_t const newKey[0x06]);

    void changeAccessBits(PN532* pn532, uint8_t sector, uint8_t const newBits[0x03], bool keyA);
    
    
    //Magic functions
    void changeUID(PN532* pn532, const uint8_t newUID[0x04], bool keyA);
    void flagMagic();
    void changeBlockZero(PN532* pn532, uint8_t const newBlock[0x10], bool keyA);
    
    /*
     clone
     
     Given a magic card, and data to clone, copies the entire contents of the data onto the magic card - including sector trailers, UID
     
     */
    void clone(PN532* pn532, const uint8_t dataIn[0x40][0x10]);
    void clone(PN532* pn532, const char* filename);


    //File I/O
    void makeFile(const char* filename);
    void loadFromFile(const char* filename, uint8_t startBlock, uint8_t endBlock);

    //Misc
    void dump();
    void flag(uint8_t block);
    
    
    //Helpers
    
    /*
     isTrailerBlock
     
     Determines whether the given block is a sector trailer or not.
     
     block: the block to check
     
     returns: true or false
     */
    static bool isTrailerBlock(uint8_t block);
    
    /*
     isFirstBlock
     
     Determines whether the given block is a the first block of a sector or not
     
     block: the block to check
     
     returns: true or false
     */
    static bool isFirstBlock(uint8_t block);
    
    /*
     isValidSector
     
     Determines whether the given sector is a valid sector number for a MIFARE 1K card
     
     sector: the sector to check
     
     returns: true or false
     */
    static bool isValidSector(uint8_t sector);
    
    static bool isValidBlock(uint8_t block);
    
    /*
     isDataBlock
     
     Determines whether the given block is a data block (i.e. not sector trailer)
     
     block: the block to check
     
     returns: true or false
     */
    static bool isDataBlock(uint8_t block);

    /*
     blockToSector
     
     Converts a given block to the sector number that it is in
     
     block: the block to convert
     
     returns: corresponding sector number
     */
    
    static uint8_t blockToSector(uint8_t block);
    
    /*
     sectorToBlock
     
     Converts a given sector to the block number of its SECTOR TRAILER
     
     sector: the sector to convert
     
     returns: corresponding sector trailer block number
     */
    static uint8_t sectorToBlock(uint8_t sector);
    
    /*
     byteToBlock
     
     Converts a given byte number to the block number of its location
     
     byte: the byte to convert
     
     returns: corresponding block number
     */
    static uint8_t byteToBlock(uint16_t byte);

    static void MIFAREInfo(PN532* pn532);
    static void trailerToConditions(uint32_t trailer);
    static uint32_t conditionsToTrailer(uint8_t conditions[4]);

protected:
    bool isMagic;

    uint8_t keysA[0x10][0x06];
    uint8_t keysB[0x10][0x06];
    uint8_t accessBits[0x10][0x03];
    uint8_t spareBytes[0x10];
    
    uint8_t UID[4];
    
    bool altered[0x40];
    
    uint8_t data[0x40][0x10];
    
    
    //Helpers
    void setDefault();
    
    /*
     paramsToData
     
     Copies the parameters (e.g. access bits, keys) into the actual data matrix of the MIFARE_1K object.  Also copies UID to block zero
     and writes the BCC
     */
    void paramsToData();
    
    
    /*
     dataToParams
     
     Copies the keyA, keyB variables from the data into their own variables (essentially opposite of above)
     */
    void dataToParams();
    
    /*
     calcBCC
     
     Calculates the BCC (uid checksum) from block zero of the card, and writes it to the fifth byte of the card
     */
    void calcBCC();
    
    








};





#endif
