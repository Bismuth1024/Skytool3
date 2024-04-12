#ifndef Skylander_H_GUARD_
#define Skylander_H_GUARD_

#include "MIFARE_1K.h"
#include <stdint.h>
#include "misc.h"
#include "Encryption.h"
#include "MD5.h"
#include "AES.h"
#include "CRC.h"
#include "toynames.h"
#include "Locations.h"
#include "Hats.h"

class Skylander : public MIFARE_1K {
	
public:
    friend class Encryption;

    //Constructors
    Skylander();
    Skylander(const char* filename);
    Skylander(PN532* nfc);
    Skylander(PN532* nfc, const char* _charCode, uint16_t _typeCode);
    
    //Get/set
    uint16_t getCharCode();
    uint16_t getTypeCode();
    
    uint16_t getGold();
    void setGold(uint16_t Gold);
    
    uint32_t getXP();
    void setXP(uint32_t XP);
    
    uint8_t getLevel();
    void setLevel(uint8_t level);
    
    uint8_t XPtoLevel(uint32_t XP);
    uint32_t levelToXP(uint8_t level);
    
    uint32_t getHeroics(uint8_t game);
    void setHeroics(uint32_t h, uint8_t game);
    
    uint16_t getPlaytime();
    void setPlaytime(uint16_t p);
    
    void getFirstPlayed(uint8_t destination[6]);
    void getLastPlayed(uint8_t destination[6]);

    std::string getHat();
    void setHat(std::string hatName);
            
    void setName(std::string newName);
    bool getName();
    
    //File I/O
    void loadBackup(const char* filename);
    
    void superchargerFormat(PN532* nfc);
    
    void printInfo();
            
            
    void setCharacter(uint16_t _charCode, uint16_t _typeCode);
            
    
    
    

    
    
protected:
    uint8_t saveArea;
    std::string Name;
    
    //Areas
    uint8_t areaBlock(uint8_t area);
    int getArea();
    
    uint64_t getValue(Locations::dataInfo location, uint8_t area);
    void setValue(Locations::dataInfo location, uint8_t area, uint64_t val);
    
    void getBytes(Locations::dataInfo location, uint8_t area, uint8_t* destination);
    void setBytes(Locations::dataInfo location, uint8_t area, uint8_t* dataIn);
};

#endif
