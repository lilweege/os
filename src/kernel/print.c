#include "print.h"
#include "font.h"

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
