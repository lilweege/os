#include "keyboard.h"
#include "screen.h"
#include "isr.h"
#include "timer.h"
#include "main.h"

void _start(void) {
    screen_init();
    isr_init();
    timer_init();
    keyboard_init();
    main();
}
