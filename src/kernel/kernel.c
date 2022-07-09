#include "keyboard.h"
#include "util.h"
#include "ports.h"
#include "screen.h"
#include "print.h"
#include "isr.h"
#include "timer.h"

void kernel_init(void) {
    screen_init();
    isr_init();
    timer_init();
    keyboard_init();
}

void _start(void) {
    kernel_init();
    for (;;) {
        u32 o = g_tick;
        for (u32 i = 0; i < SCREEN_HEIGHT; ++i)
            for (u32 j = 0; j < SCREEN_WIDTH; ++j) {
                screen_set_px(j, (i+o)%SCREEN_HEIGHT, i&0xFF);
            }
        u16 x=0,y=0;
        if (g_kbkeys[KEY_UP]) print_cstr(&x, &y, "UP");
        y += 16; x = 0;
        if (g_kbkeys[KEY_DOWN]) print_cstr(&x, &y, "DOWN");
        y += 16; x = 0;
        if (g_kbmods & KEY_MOD_SHIFT) print_cstr(&x, &y, "SHIFT");
        y += 16; x = 0;
        if (g_kbchars['a']) print_cstr(&x, &y, "A");
        y += 16; x = 0;
        screen_swap_buffers();
    }

    for(;;) asm("hlt");
}
