#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "types.h"

#define KEY_ESCAPE           0x01
#define KEY_LCTRL            0x1D
#define KEY_RCTRL            0x1D
#define KEY_LSHIFT           0x2A
#define KEY_RSHIFT           0x36
#define KEY_PAD_MUL          0x37
#define KEY_LALT             0x38
#define KEY_RALT             0x38
#define KEY_CAPSLOCK         0x3A
#define KEY_F1               0x3B
#define KEY_F2               0x3C
#define KEY_F3               0x3D
#define KEY_F4               0x3E
#define KEY_F5               0x3F
#define KEY_F6               0x40
#define KEY_F7               0x41
#define KEY_F8               0x42
#define KEY_F9               0x43
#define KEY_F10              0x44
#define KEY_NUMLOCK          0x45
#define KEY_SCROLLLOCK       0x46
#define KEY_HOME             0x47
#define KEY_UP               0x48
#define KEY_PAGE_UP          0x49
#define KEY_PAD_SUB          0x4A
#define KEY_LEFT             0x4B
#define KEY_RIGHT            0x4D
#define KEY_PAD_ADD          0x4E
#define KEY_END              0x4F
#define KEY_DOWN             0x50
#define KEY_PAGE_DOWN        0x51
#define KEY_INSERT           0x52
#define KEY_DELETE           0x53
#define KEY_F11              0x57
#define KEY_F12              0x58
#define KEY_META             0x5B

#define SCANCODE_UP          0x80
#define SCANCODE_PRESSED(x)  (((x) & SCANCODE_UP) == 0)
#define KEY_SCANCODE(x)      ((x) & (SCANCODE_UP - 1))


typedef enum {
    KEY_MOD_META       = 0b00000001,
    KEY_MOD_CTRL       = 0b00000010,
    KEY_MOD_SHIFT      = 0b00000100,
    KEY_MOD_ALT        = 0b00001000,
    KEY_MOD_CAPSLOCK   = 0b00010000,
    KEY_MOD_NUMLOCK    = 0b00100000,
    KEY_MOD_SCROLLLOCK = 0b01000000,
} KeyMod;


extern u8 g_kbmods;
extern u8 g_kbkeys[128];
extern u8 g_kbchars[128];

void keyboard_init(void);

#endif // KEYBOARD_H_
