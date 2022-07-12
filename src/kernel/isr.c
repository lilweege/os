#include "isr.h"

#include "idt.h"
#include "ports.h"
#include "print.h"
#include "screen.h"


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

void isr_handler(registers_t registers) {
    u16 x = 0, y = 0;
    print_cstr(&x, &y, "Exception (");
    print_num(&x, &y, registers.int_no);
    print_cstr(&x, &y, "): ");
    print_cstr(&x, &y, exception_messages[registers.int_no]);
    print_cstr(&x, &y, " ");
    if (registers.err_code) {
        print_num(&x, &y, registers.err_code);
    }
    screen_swap_buffers();
    for (;;)
        ;
}

static isr_t irq_handlers[16];
#define PIC1      0x20 // IO base address for master PIC
#define PIC2      0xA0 // IO base address for slave PIC
#define PIC1_CMD  PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_CMD  PIC2
#define PIC2_DATA (PIC2 + 1)

#define PIC_EOI   0x20

// initialization control words
#define ICW1_ICW4       0x01 // ICW4 (not) needed
#define ICW1_SINGLE     0x02 // Single (cascade) mode
#define ICW1_INTERVAL4  0x04 // Call address interval 4 (8)
#define ICW1_LEVEL      0x08 // Level triggered (edge) mode
#define ICW1_INIT       0x10 // Initialization - required!

#define ICW4_8086       0x01 // 8086/88 (MCS-80/85) mode
#define ICW4_AUTO       0x02 // Auto (normal) EOI
#define ICW4_BUF_SLAVE  0x08 // Buffered mode/slave
#define ICW4_BUF_MASTER 0x0C // Buffered mode/master
#define ICW4_SFNM       0x10 // Special fully nested (not)

static void pic_remap(void) {
    // NOTE: io_wait calls are probably not entirely necessary

    // save current masks
    u8 a1 = inb(PIC1_DATA);
    u8 a2 = inb(PIC2_DATA);

    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);

    // ICW2: idt vector offsets
    outb(PIC1_DATA, 32);
    outb(PIC2_DATA, 32 + 8);
    // io_wait();

    // ICW3: base int in idt
    outb(PIC1_DATA, 4); // tell master that there is a slave PIC at IRQ2 (0000 0100)
    // io_wait();
    outb(PIC2_DATA, 2); // tell slave its cascade identity (0000 0010)
    // io_wait();

    // ICW4: x86 mode
    outb(PIC1_DATA, ICW4_8086);
    // io_wait();
    outb(PIC2_DATA, ICW4_8086);
    // io_wait();

    // restore masks
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void irq_handler(registers_t registers) {
    // send end of input or they will not send any more interrupts
    if (registers.int_no >= 32 + 8)
        outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);

    if (irq_handlers[registers.int_no - 32])
        irq_handlers[registers.int_no - 32](registers);
}


static void* isrs[NUM_IDT_ENTRIES] = {
    isr0,  isr1,  isr2,  isr3,  isr4,  isr5,  isr6,  isr7,  isr8,  isr9,  isr10, isr11,
    isr12, isr13, isr14, isr15, isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
    isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31, irq0,  irq1,  irq2,  irq3,
    irq4,  irq5,  irq6,  irq7,  irq8,  irq9,  irq10, irq11, irq12, irq13, irq14, irq15,
};

void irq_install(u32 n, isr_t handler) {
    asm volatile("cli");
    irq_handlers[n] = handler;
    asm volatile("sti");
}

void isr_init(void) {
    pic_remap();
    for (u32 i = 0; i < 48; ++i) {
        idt_set_entry(i, isrs[i], INT_GATE_FLAGS);
    }
    idt_load();
    asm volatile("sti");
}
