#ifndef TOYNAMES_H_GUARD_
#define TOYNAMES_H_GUARD_

/*
 Here we store all the character to code conversions.
 
 This is tricky for a couple of reasons
    -Each figure has two 2-byte codes - one specifies the character, the other the variant
    -Usually different variants of the same character have the same character code but different variant codes
    -But this is not always the case - e.g. dark spyro and series 1 spyro have different character codes instead
    -Also, variant codes are very inconsistent - there are some patterns like all non legendaries are 0x0000 for Spyro's adventure,
    but it all goes downhill from there and seems almost but not quite random by the later games
    -Character codes also become inconsistent, they are consecutive in SA but are strange in later games
    -I want a two way lookup so we can quickly convert between codes and name (when reading figure) and name and codes (when writing a new figure)
 
 
 
 
 */

#include <map>
#include <string>

void loadNames();
void fillCodes(uint16_t* startPtr, uint16_t val, uint16_t n);
uint16_t convertCharName(const char* name);
std::string getCharName(uint16_t code);








#endif
