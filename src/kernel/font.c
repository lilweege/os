#include "font.h"
#include "screen.h"

void draw_char(u32 x, u32 y, u8 c) {
    if (ASCII_PRINTABLE_MIN <= c && c <= ASCII_PRINTABLE_MAX) {
        // assert(0 <= x && x+FontCharWidth <= SCREEN_WIDTH);
        // assert(0 <= y && y+FontCharHeight <= SCREEN_HEIGHT);
        for (u32 i = 0; i < FontCharHeight; ++i) {
            for (u32 j = 0; j < FontCharWidth/8; ++j) {
                u8 b = FontBitmap[(c-ASCII_PRINTABLE_MIN)*FontCharHeight*FontCharWidth/8 + i*FontCharWidth/8+j];
                for (u32 s = 0; s < 8; ++s) {
                    if (b & (1U << (7-s)))
                        screen_set_px(x+j*8+s, y+i, 0xFF);
                }
            }
        }
    }
}