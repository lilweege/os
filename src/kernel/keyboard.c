#include "keyboard.h"

#include "isr.h"
#include "ports.h"


u8 g_kbmods;
u8 g_kbkeys[128];
u8 g_kbchars[128];

// clang-format off
u8 us_qwerty[2][128] = {
    {
        0, [KEY_ESCAPE]=0x1B, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', [KEY_LCTRL]=0,
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', [KEY_LSHIFT]=0, '\\',
        'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', [KEY_RSHIFT]=0,
        [KEY_PAD_MUL]='*', [KEY_LALT]=0, ' ', [KEY_CAPSLOCK]=0,
        [KEY_F1]=0, [KEY_F2]=0, [KEY_F3]=0, [KEY_F4]=0, [KEY_F5]=0, [KEY_F6]=0, [KEY_F7]=0, [KEY_F8]=0, [KEY_F9]=0, [KEY_F10]=0,
        [KEY_NUMLOCK]=0, [KEY_SCROLLLOCK]=0,
        [KEY_HOME]='7', [KEY_UP]='8', [KEY_PAGE_UP]='9', [KEY_PAD_SUB]='-',
        [KEY_LEFT]='4', '5', [KEY_RIGHT]='6', [KEY_PAD_ADD]='+',
        [KEY_END]='1', [KEY_DOWN]='2', [KEY_PAGE_DOWN]='3', [KEY_INSERT]='0', [KEY_DELETE]='.',
        0, 0, 0, [KEY_F11]=0, [KEY_F12]=0
    }, {
        0, [KEY_ESCAPE]=0x1B, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
        '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', [KEY_LCTRL]=0,
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', [KEY_LSHIFT]=0, '|',
        'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', [KEY_RSHIFT]=0,
        [KEY_PAD_MUL]='*', [KEY_LALT]=0, ' ', [KEY_CAPSLOCK]=0,
        [KEY_F1]=0, [KEY_F2]=0, [KEY_F3]=0, [KEY_F4]=0, [KEY_F5]=0, [KEY_F6]=0, [KEY_F7]=0, [KEY_F8]=0, [KEY_F9]=0, [KEY_F10]=0,
        [KEY_NUMLOCK]=0, [KEY_SCROLLLOCK]=0,
        [KEY_HOME]='7', [KEY_UP]='8', [KEY_PAGE_UP]='9', [KEY_PAD_SUB]='-',
        [KEY_LEFT]='4', '5', [KEY_RIGHT]='6', [KEY_PAD_ADD]='+',
        [KEY_END]='1', [KEY_DOWN]='2', [KEY_PAGE_DOWN]='3', [KEY_INSERT]='0', [KEY_DELETE]='.',
        0, 0, 0, [KEY_F11]=0, [KEY_F12]=0
    }
};
// clang-format on

static void keyboard_handler(registers_t registers) {
    (void) registers;
    u8 key = inb(0x60);
    u8 pressed = SCANCODE_PRESSED(key);
    u8 scancode = KEY_SCANCODE(key);

    if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
        g_kbmods ^= KEY_MOD_SHIFT;
    }
    else if (scancode == KEY_LCTRL || scancode == KEY_RCTRL) {
        g_kbmods ^= KEY_MOD_CTRL;
    }
    else if (scancode == KEY_LALT || scancode == KEY_RALT) {
        g_kbmods ^= KEY_MOD_ALT;
    }
    else if (scancode == KEY_META) {
        g_kbmods ^= KEY_MOD_META;
    }
    else if (pressed && scancode == KEY_CAPSLOCK) {
        g_kbmods ^= KEY_MOD_CAPSLOCK; // mostly unused
        g_kbmods ^= KEY_MOD_SHIFT;
    }
    else if (pressed && scancode == KEY_NUMLOCK) {
        g_kbmods ^= KEY_MOD_NUMLOCK;
    }
    else if (pressed && scancode == KEY_SCROLLLOCK) {
        g_kbmods ^= KEY_MOD_SCROLLLOCK;
    }

    // NOTE: only using lowercase set in kbchars
    // u8 shift = (g_kbmods & KEY_MOD_SHIFT) != 0;
    u8 ascii = us_qwerty[0][scancode];
    g_kbkeys[scancode] = pressed;
    g_kbchars[ascii] = pressed;
}


void keyboard_init(void) {
    irq_install(1, keyboard_handler);
}
