#include "timer.h"

#include "isr.h"
#include "ports.h"
#include "print.h"
#include "screen.h"
#include "util.h"


#define PIT_A    0x40
#define PIT_B    0x41
#define PIT_C    0x42
#define PIT_CTRL 0x43

#define PIT_SET  0x36
#define PIT_HZ   1193181
#define IRQ_FREQ 1000

u32 g_tick = 0;
static void timer_handler(registers_t registers) {
    (void) registers;
    ++g_tick;
}

void timer_init(void) {
    u16 d = (u16) (PIT_HZ / IRQ_FREQ);
    outb(PIT_CTRL, PIT_SET);
    outb(PIT_A, d & 0xFF);
    outb(PIT_A, (d >> 8) & 0xFF);

    irq_install(0, timer_handler);
}