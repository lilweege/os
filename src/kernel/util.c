#include "util.h"


void memcpy(void* dst, const void* src, i32 n) {
    u8* a = dst;
    const u8* b = src;
    while (n-- > 0)
        *a++ = *b++;
}

void memset(void* s, u8 c, size_t n) {
    u8* x = s;
    while (n-- > 0)
        *x++ = c;
}

void itoa(i32 x, char* s) {
    const u8 sz = 32;
    char buf[sz];
    u8 i = sz-1;
    u8 neg = x < 0;
    if (neg) x = -x;
    do {
        buf[i--] = '0' + (x % 10);
    } while (x /= 10);
    if (neg)
        buf[i--] = '-';
    ++i;
    s[sz-i] = 0;
    memcpy(s, buf+i, sz-i);
}

void htoa(u32 x, char* s) {
    const u8 sz = 32;
    char buf[sz];
    u8 i = sz-1;
    do {
        u8 lo = (x & 0x0F);
        u8 hi = (x & 0xF0) >> 4;
        buf[i--] = lo < 10 ? (lo+'0') : (lo-10+'A');
        buf[i--] = hi < 10 ? (hi+'0') : (hi-10+'A');
    } while (x >>= 8);
    buf[i--] = 'x';
    buf[i--] = '0';
    ++i;
    s[sz-i] = 0;
    memcpy(s, buf+i, sz-i);
}
