#ifndef MOUSE_H
#define MOUSE_H

#include "types.h"


typedef enum {
    MOUSE_BTN_LEFT = 0x01,
    MOUSE_BTN_MIDDLE = 0x02,
    MOUSE_BTN_RIGHT = 0x04,
    // MOUSE_BTN_X1 = 0x08,
    // MOUSE_BTN_X2 = 0x10,
} MouseButton;

extern u8 g_mbtn;
extern i32 g_mx, g_my;

void mouse_init(void);

#endif // MOUSE_H
