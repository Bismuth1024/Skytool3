//
//  main.cpp
//  Skytool3
//
//  Created by Manith Kha on 13/3/2024.
//

#include <stdio.h>
#include "misc.h"
#include "md5.h"

int main() {
    uint8_t destination[16];
    
    MD5::hash(destination, "test", 4);
    printHexBytes(destination, 16, -1);
    
    
    return 0;
    
    
    return 0;
}
