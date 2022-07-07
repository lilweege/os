#include "screen.h"
#include "ports.h"
#include "util.h"

u8 _sbuff[2][SCREEN_SIZE];
u8 _sbuff_front = 0;

#define VGA_OFFSET     ((u8*)0xA0000)
#define PALETTE_MASK   0x3C6
#define PALETTE_READ   0x3C7
#define PALETTE_WRITE  0x3C8
#define PALETTE_DATA   0x3C9

static void screen_configure_palette() {
    // 8-bit RRRGGGBB color
    port_byte_out(PALETTE_MASK, 0xFF);
    port_byte_out(PALETTE_WRITE, 0);
    for (i32 i = 0; i < 255; ++i) {
        port_byte_out(PALETTE_DATA, (((i >> 5) & 0x7) * (256 / 8)) / 4);
        port_byte_out(PALETTE_DATA, (((i >> 2) & 0x7) * (256 / 8)) / 4);
        port_byte_out(PALETTE_DATA, (((i >> 0) & 0x3) * (256 / 4)) / 4);
    }
    // set color 255 = white
    port_byte_out(PALETTE_DATA, 0x3F);
    port_byte_out(PALETTE_DATA, 0x3F);
    port_byte_out(PALETTE_DATA, 0x3F);
}

void screen_swap_buffers() {
    memcpy(VGA_OFFSET, &_sbuff[_sbuff_front], SCREEN_SIZE);
    _sbuff_front ^= 1;
}

void screen_init() {
    screen_configure_palette();
}
