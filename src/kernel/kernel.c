#include "isr.h"
#include "keyboard.h"
#include "main.h"
#include "mouse.h"
#include "screen.h"
#include "timer.h"


void _start(void) {
    screen_init();
    isr_init();
    timer_init();    // irq0
    keyboard_init(); // irq1
    mouse_init();    // irq12

    kmain();
}
