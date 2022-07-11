#include "main.h"

#include "font.h"
#include "keyboard.h"
#include "mouse.h"
#include "print.h"
#include "screen.h"
#include "timer.h"
#include "types.h"

// clang-format off
static u16 mouseCursor[10*16] = {
    0xFF00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFF00, 0xFF00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFF00, 0xFFFF, 0xFF00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFF00, 0xFFFF, 0xFFFF, 0xFF00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFF00, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFF00, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF00, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFF00, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF00, 0x0000, 0x0000, 0x0000,
    0xFF00, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF00, 0x0000, 0x0000,
    0xFF00, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF00, 0x0000,
    0xFF00, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF00,
    0xFF00, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFF00, 0xFF00, 0xFF00, 0xFF00,
    0xFF00, 0xFFFF, 0xFFFF, 0xFF00, 0xFFFF, 0xFFFF, 0xFF00, 0x0000, 0x0000, 0x0000,
    0xFF00, 0xFFFF, 0xFF00, 0x0000, 0xFF00, 0xFFFF, 0xFF00, 0x0000, 0x0000, 0x0000,
    0xFF00, 0xFF00, 0x0000, 0x0000, 0x0000, 0xFF00, 0xFFFF, 0xFF00, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFF00, 0xFFFF, 0xFF00, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFF00, 0xFF00, 0x0000, 0x0000,
};
// clang-format on

static void update(u32 dt) {
    static u32 o = 0;
    o += dt;
    for (u32 i = 0; i < SCREEN_HEIGHT; ++i)
        for (u32 j = 0; j < SCREEN_WIDTH; ++j) {
            screen_set_px(j, (i + o) % SCREEN_HEIGHT, i & 0xFF);
        }
    u16 x = 0, y = 0;
    print_num(&x, &y, g_mbtn);
    y += 16, x = 0;
    if (0 <= g_mx && g_mx < SCREEN_WIDTH && 0 <= g_my && g_my < SCREEN_HEIGHT)
        screen_draw_bitmap(mouseCursor, g_mx, g_my, 10, 16);
    if (g_kbkeys[KEY_UP])
        print_cstr(&x, &y, "UP");
    y += 16, x = 0;
    if (g_kbkeys[KEY_DOWN])
        print_cstr(&x, &y, "DOWN");
    y += 16, x = 0;
    if (g_kbmods & KEY_MOD_SHIFT)
        print_cstr(&x, &y, "SHIFT");
    y += 16, x = 0;
    if (g_kbchars['a'])
        print_cstr(&x, &y, "A");
    y += 16, x = 0;
    screen_swap_buffers();
}

void main(void) {
    u32 lastTick = 0;
    while (1) {
        // TODO: throttling
        u32 currTick = g_tick;
        update(currTick - lastTick);
        lastTick = currTick;
    }
}
