#ifndef IDT_H_
#define IDT_H_

#include "extra/idt.h"
#include "types.h"
#define NUM_IDT_ENTRIES 256
#define INT_GATE_FLAGS  0b10001110
#define TRAP_GATE_FLAGS 0b10001111

// each interrupt handler (gate)
typedef struct {
    u16 isr_low;      // The lower 16 bits of the ISR's address
    u16 kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
    u8  zero;         // Reserved
    u8  flags;        // Type and attributes; see the IDT page
    u16 isr_high;     // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;


// pointer to idt
// used with asm isntruction 'lidt'
typedef struct {
    u16 limit;
    uintptr_t base;
} __attribute__((packed)) idt_register_t;

extern idt_entry_t g_idt[NUM_IDT_ENTRIES];
extern idt_register_t g_idrt;

void idt_load(void);
void idt_set_entry(u8 vector, void* isr, u8 flags);

#endif // IDT_H_
