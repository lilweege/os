#include "screen.h"

#include "ports.h"
#include "util.h"


u8 g_sbuff[2][SCREEN_SIZE];
u8 g_sbuff_front = 0;

#define VGA_OFFSET    ((u8*) 0xA0000)
#define PALETTE_MASK  0x3C6
#define PALETTE_READ  0x3C7
#define PALETTE_WRITE 0x3C8
#define PALETTE_DATA  0x3C9

static void screen_configure_palette(void) {
    // 8-bit RRRGGGBB color
    outb(PALETTE_MASK, 0xFF);
    outb(PALETTE_WRITE, 0);
    for (i32 i = 0; i < 255; ++i) {
        outb(PALETTE_DATA, (((i >> 5) & 0x7) * (256 / 8)) / 4);
        outb(PALETTE_DATA, (((i >> 2) & 0x7) * (256 / 8)) / 4);
        outb(PALETTE_DATA, (((i >> 0) & 0x3) * (256 / 4)) / 4);
    }
    // set color 255 = white
    outb(PALETTE_DATA, 0x3F);
    outb(PALETTE_DATA, 0x3F);
    outb(PALETTE_DATA, 0x3F);
}

void screen_draw_bitmap(const u16* buff, u16 x, u16 y, u16 width, u16 height) {
    for (u32 i = 0; i < height; ++i) {
        for (u32 j = 0; j < width; ++j) {
            u8 yes = (buff[i * width + j] & 0xFF00) != 0;
            if (yes) {
                u8 col = buff[i * width + j] & 0xFF;
                u16 sx = x + j;
                u16 sy = y + i;
                if (0 <= sx && sx < SCREEN_WIDTH && 0 <= sy && sy < SCREEN_HEIGHT)
                    screen_set_px(sx, sy, col);
            }
        }
    }
}

void screen_draw_bitmap_mono(const u8* buff, u16 x, u16 y, u16 width, u16 height) {
    for (u32 i = 0; i < height; ++i) {
        for (u32 j = 0; j < width / 8; ++j) {
            u8 b = buff[i * width / 8 + j];
            for (u32 s = 0; s < 8; ++s) {
                if (b & (1U << (7 - s))) {
                    u16 sx = x + j * 8 + s;
                    u16 sy = y + i;
                    if (0 <= sx && sx < SCREEN_WIDTH && 0 <= sy && sy < SCREEN_HEIGHT)
                        screen_set_px(sx, sy, 0xFF);
                }
            }
        }
    }
}

void screen_fill(u8 col) {
    memset(g_sbuff[g_sbuff_front], col, SCREEN_SIZE);
}

void screen_swap_buffers(void) {
    memcpy(VGA_OFFSET, &g_sbuff[g_sbuff_front], SCREEN_SIZE);
    g_sbuff_front ^= 1;
}

void screen_init(void) {
    screen_configure_palette();
}
