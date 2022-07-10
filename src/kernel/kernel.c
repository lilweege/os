#include "isr.h"
#include "keyboard.h"
#include "main.h"
#include "screen.h"
#include "timer.h"


void _start(void) {
    screen_init();
    isr_init();
    timer_init();
    keyboard_init();
    main();
}
