#include "idt.h"

#include "types.h"


idt_entry_t idt[NUM_IDT_ENTRIES];
idt_register_t idtr;

void idt_load(void) {
    idtr = (idt_register_t){
        .limit = sizeof(idt) - 1,
        .base = (u32) &idt,
    };
    asm volatile("lidt %0" : : "m"(idtr));
}

void idt_set_entry(u8 vector, void* isr, u8 flags) {
    uintptr_t handler = (uintptr_t) isr;
    idt[vector] = (idt_entry_t){
        .isr_low = handler & 0xFFFF,
        .isr_high = handler >> 16,
        .kernel_cs = 0x08,
        .flags = flags,
    };
}
