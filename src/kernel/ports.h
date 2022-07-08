#ifndef PORTS_H_
#define PORTS_H_

#include "types.h"

static inline u8 inb(u16 port) {
    u8 result;
    asm volatile("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

static inline void outb(u16 port, u8 data) {
    asm volatile("out %%al, %%dx" : : "a" (data), "d" (port));
}

static inline void io_wait(void) {
    outb(0x80, 0);
}

#endif // PORTS_H
