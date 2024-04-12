#ifndef _CRC_H_
#define _CRC_H_

#include <stdint.h>
#include "misc.h"

/*

The CRC class is a very basic class.

width: Number of bits in the CRC (16,48,64, etc)
polynomial: The polynomial of the CRC
initial: Initial register value for the CRC



*/

class CRC {

  public:
    CRC(uint8_t width, uint64_t polynomial, uint64_t initial);
    
    void compute(uint8_t* input, int nBytes, uint8_t* destination);
    
  private:
    uint8_t width;
    uint64_t polynomial;
    uint64_t initial;
    



  
};








#endif
