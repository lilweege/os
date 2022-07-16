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
                if (sx < SCREEN_WIDTH && sy < SCREEN_HEIGHT)
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
                    if (sx < SCREEN_WIDTH && sy < SCREEN_HEIGHT)
                        screen_set_px(sx, sy, 0xFF);
                }
            }
        }
    }
}

void screen_draw_line(i32 x1, i32 y1, i32 x2, i32 y2, u8 col) {
    i32 dx = x2 - x1;
    i32 dy = y2 - y1;

    // simple lines
    if (dx == 0) {
        if (y2 < y1) {
            i32 t = y2;
            y2 = y1;
            y1 = t;
        }
        for (i32 y = y1; y <= y2; ++y)
            if (0 <= x1 && x1 < SCREEN_WIDTH && 0 <= y && y < SCREEN_HEIGHT)
                screen_set_px(x1, y, col);
    }
    else if (dy == 0) {
        if (x2 < x1) {
            i32 t = x2;
            x2 = x1;
            x1 = t;
        }
        for (i32 x = x1; x <= x2; ++x)
            if (0 <= x && x < SCREEN_WIDTH && 0 <= y1 && y1 < SCREEN_HEIGHT)
                screen_set_px(x, y1, col);
    }
    else {
        u8 dxPos = dx > 0;
        u8 dyPos = dy > 0;
        i32 dx1 = dxPos ? dx : -dx;
        i32 dy1 = dyPos ? dy : -dy;
        i32 px = 2 * dy1 - dx1;
        i32 py = 2 * dx1 - dy1;
        i32 x, y, xe, ye;
        if (dy1 <= dx1) {
            if (dx >= 0) {
                x = x1;
                y = y1;
                xe = x2;
            }
            else {
                x = x2;
                y = y2;
                xe = x1;
            }
            if (0 <= x && x < SCREEN_WIDTH && 0 <= y && y < SCREEN_HEIGHT)
                screen_set_px(x, y, col);
            for (i32 i = 0; x < xe; ++i) {
                ++x;
                if (px < 0) {
                    px += 2 * dy1;
                }
                else {
                    y += dxPos == dyPos ? 1 : -1;
                    px += 2 * (dy1 - dx1);
                }
                if (0 <= x && x < SCREEN_WIDTH && 0 <= y && y < SCREEN_HEIGHT)
                    screen_set_px(x, y, col);
            }
        }
        else {
            if (dy >= 0) {
                x = x1;
                y = y1;
                ye = y2;
            }
            else {
                x = x2;
                y = y2;
                ye = y1;
            }
            if (0 <= x && x < SCREEN_WIDTH && 0 <= y && y < SCREEN_HEIGHT)
                screen_set_px(x, y, col);
            for (i32 i = 0; y < ye; ++i) {
                ++y;
                if (py <= 0) {
                    py += 2 * dx1;
                }
                else {
                    x += dxPos == dyPos ? 1 : -1;
                    py += 2 * (dx1 - dy1);
                }
                if (0 <= x && x < SCREEN_WIDTH && 0 <= y && y < SCREEN_HEIGHT)
                    screen_set_px(x, y, col);
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
