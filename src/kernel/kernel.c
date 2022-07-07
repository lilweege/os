#include "util.h"
#include "ports.h"
#include "screen.h"
#include "print.h"
#include "isr.h"

void kernel_init() {
    isr_init();
    screen_init();
}

#define SIZE 1000*100
static u8 filler [SIZE] = {[0 ... SIZE-1]='A'};

void _start(void) {
    kernel_init();

    u16 x=0, y=0;
    for (u32 o = 0; o < 10000; ++o) {
        x=0,y=0;
        for (u32 i = 0; i < SCREEN_HEIGHT; ++i)
            for (u32 j = 0; j < SCREEN_WIDTH; ++j) {
                screen_set_px(j, (i+o)%SCREEN_HEIGHT, i&0xFF);
            }
        if (filler[SIZE-1] != 'A') {
            print_cstr(&x, &y, "MEM ERROR!");
        }
        else {
            print_cstr(&x, &y, "OK!");
        }
        screen_swap_buffers();

        if (o == 1000) {
            asm volatile("int $0x02");
        }
    }

    for(;;) asm("hlt");
}
