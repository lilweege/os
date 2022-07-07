#include "isr.h"
#include "idt.h"
#include "screen.h"
#include "print.h"
#include "util.h"



typedef struct {
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 sp;
    u32 ss;
} __attribute__((packed)) int_frame_t;


__attribute__((interrupt))
void exception_handler(int_frame_t* frame) {
    for(;;);
}

__attribute__((interrupt))
void exception_handler_errorcode(int_frame_t* frame, u32 error) {
    for(;;);
}

__attribute__((interrupt))
void interrupt_handler(int_frame_t* frame) {
    for(;;);
}

static char* exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};


static void isr_install() {
    // TODO: each exception/interrupt should have individual functions
    u32 i = 0;
    for (; i < 32; ++i) {
        if (i == 8 || i == 10 || i == 11 || i == 12 || i == 13 || i == 14 || i == 17 || i == 21) {
            // error code parameter
            idt_set_entry(i, exception_handler_errorcode, TRAP_GATE_FLAGS);
        }
        else {
            // no parameters
            idt_set_entry(i, exception_handler, TRAP_GATE_FLAGS);
        }
    }
    for (; i < 256; ++i) {
        idt_set_entry(i, interrupt_handler, INT_GATE_FLAGS);
    }
    idt_load();
    //asm volatile("sti");
}

void isr_init() {
    isr_install();
}
