#include "mouse.h"

#include "isr.h"
#include "ports.h"
#include "screen.h"


u8 g_mbtn;
i32 g_mx, g_my;

typedef struct {
    u8 leftBtn   : 1;
    u8 rightBtn  : 1;
    u8 middleBtn : 1;
    u8 one       : 1;
    u8 xSign     : 1;
    u8 ySign     : 1;
    u8 xOverflow : 1;
    u8 yOverflow : 1;
} MousePacketInfo;

typedef struct {
    union {
        u8 _info;
        MousePacketInfo info;
    };
    u8 xMovement;
    u8 yMovement;
} MousePacket;

static void mouse_wait(void) {
    for (u32 timeout = 100000; --timeout;) {
        if ((inb(0x64) & 0b10) == 0)
            return;
    }
}

static void mouse_wait_input(void) {
    for (u32 timeout = 100000; --timeout;) {
        if ((inb(0x64) & 0b01) != 0)
            return;
    }
}

static void mouse_write(u8 val) {
    mouse_wait();
    outb(0x64, 0xD4);
    mouse_wait();
    outb(0x60, val);
}

static u8 mouse_read(void) {
    mouse_wait_input();
    return inb(0x60);
}

static void mouse_handle_packet(MousePacket packet) {
    if (!packet.info.one || packet.info.yOverflow || packet.info.xOverflow)
        return; // the packet is probably bad

    g_mbtn = 0;
    if (packet.info.leftBtn)
        g_mbtn |= MOUSE_BTN_LEFT;
    if (packet.info.rightBtn)
        g_mbtn |= MOUSE_BTN_RIGHT;
    if (packet.info.middleBtn)
        g_mbtn |= MOUSE_BTN_MIDDLE;

    g_mx += packet.info.xSign ? -((i32) 0xFF - packet.xMovement) : packet.xMovement;
    g_my += packet.info.ySign ? 0xFF - packet.yMovement : -((i32) packet.yMovement);

    if (g_mx < 0)
        g_mx = 0;
    if (g_mx > SCREEN_WIDTH - 1)
        g_mx = SCREEN_WIDTH - 1;

    if (g_my < 0)
        g_my = 0;
    if (g_my > SCREEN_HEIGHT - 1)
        g_my = SCREEN_HEIGHT - 1;
}

static void mouse_handler(registers_t registers) {
    (void) registers;
    static u8 mouseCycle = 0;
    static MousePacket packet;

    u8 data = mouse_read();
    switch (mouseCycle) {
        case 0: {
            packet._info = data;
            ++mouseCycle;
        } break;
        case 1: {
            packet.xMovement = data;
            ++mouseCycle;
        } break;
        case 2: {
            packet.yMovement = data;
            mouseCycle = 0;
            mouse_handle_packet(packet);
            packet = (MousePacket){0};
        } break;
    }
}

void mouse_init(void) {
    // enable auxiliary device
    mouse_wait();
    outb(0x64, 0xA8);

    // get compaq status byte
    mouse_wait();
    outb(0x64, 0x20);
    mouse_wait_input();
    u8 status = inb(0x60);
    status |= 0b10;  // enable irq12
    status &= ~0x20; // disable mouse clock

    // set compaq status
    mouse_wait();
    outb(0x64, 0x60);
    mouse_wait();
    outb(0x60, status);

    u8 response;
    mouse_write(0xF6); // set defaults
    response = mouse_read();
    while (response != 0xFA) // ACK
        ;

    mouse_write(0xF4); // enable data reporting
    response = mouse_read();
    while (response != 0xFA) // ACK
        ;

    irq_install(12, mouse_handler);
}
