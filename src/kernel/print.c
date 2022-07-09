#include "print.h"
#include "font.h"
#include "util.h"

void print_cstr(u16* x, u16* y, const char* s) {
    // TODO: edge wrapping
    u8 c;
    while ((c = *s++)) {
        draw_char(*x, *y, c);
        *x += 8;
        if (*x >= SCREEN_WIDTH) {
            *x = 0;
            *y += 16;
        }
    }
}

void print_num(u16* x, u16* y, i32 n) {
    static char b[32];
    itoa(n, b);
    print_cstr(x, y, b);
}

void print_hex(u16* x, u16* y, u32 n) {
    static char b[32];
    htoa(n, b);
    print_cstr(x, y, b);
}
