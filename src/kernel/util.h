#ifndef UTIL_H_
#define UTIL_H_

#include "types.h"

void memcpy(void* dst, const void* src, i32 n);
void memset(void* s, u8 c, size_t n);
void itoa(i32 x, char* s);

#endif // UTIL_H_
