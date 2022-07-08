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
}

void _start(void) {
    kernel_init();
    for (u32 o = 0; o < 100000; ++o) {
        for (u32 i = 0; i < SCREEN_HEIGHT; ++i)
            for (u32 j = 0; j < SCREEN_WIDTH; ++j) {
                screen_set_px(j, (i+o)%SCREEN_HEIGHT, i&0xFF);
            }
        screen_swap_buffers();
    }

    for(;;) asm("hlt");
}
