//
//  exceptions.h
//  Skytool3
//
//  Created by Manith Kha on 14/3/2024.
//

#ifndef EXCEPTIONS_H_GUARD
#define EXCEPTIONS_H_GUARD

#include <exception>

class CodedException: public std::exception
{
private:
    uint16_t code;
    
public:
    CodedException(uint16_t code);
    const char * what();
};

#endif /* exceptions_hpp */
