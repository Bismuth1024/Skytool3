/* md5.h - header file for md5.c */
/* RSA Data Security, Inc., MD5 Message-Digest Algorithm */

/* NOTE: Numerous changes have been made; the following notice is
included to satisfy legal requirements.

Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
*/
#ifndef MD5_H_GUARD_
#define MD5_H_GUARD_

#include <memory.h>
#include "misc.h"

class MD5 {
public:
    MD5();
    void compute(uint8_t *output, const void *bytesIn, unsigned int inputLen);
    static void hash(uint8_t *output, const void *bytesIn, unsigned int inputLen);

private:
    uint32_t state[4];
    uint32_t count[2];
    uint8_t buffer[64];
    
    void transform(const uint8_t block[64]);
    void digest(const void *input, uint32_t inputLen);
    void close(uint8_t digest[16]);
};

#endif
