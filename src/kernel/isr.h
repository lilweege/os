#ifndef ISR_H_
#define ISR_H_

#include "types.h"

typedef struct {
    u32 err;
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 sp;
    u32 ss;
} __attribute__((packed)) int_frame_err_t;

typedef struct {
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 sp;
    u32 ss;
} __attribute__((packed)) int_frame_t;


typedef void(*isr_t)(void);

void irq_install(u32 n, isr_t handler);
void isr_init(void);

#endif // ISR_H_
