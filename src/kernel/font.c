#include "font.h"

#include "screen.h"


void draw_char(u32 x, u32 y, u8 c) {
    if (ASCII_PRINTABLE_MIN <= c && c <= ASCII_PRINTABLE_MAX) {
        u32 charOffset = (c - ASCII_PRINTABLE_MIN) * FontCharHeight * FontCharWidth / 8;
        screen_draw_bitmap_mono(FontBitmap + charOffset, x, y, FontCharWidth, FontCharHeight);
    }
}
