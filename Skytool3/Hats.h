/*
 Basically just a list of all the hats across the games
 
 hats are identified by a 2 byte code
 */

#ifndef HATS_H_GUARD_
#define HATS_H_GUARD_

#include "misc.h"
#include <map>
#include <string>

//Honestly all of these should be mostly self-explanatory
std::string getHatName(uint16_t code);

uint8_t getHatGame(uint16_t code); //0 spyro -> 5 superchargers

uint16_t getHatCode(std::string hatName); //currently returns 0xffff if not found


#endif
