#ifndef SCREEN_H_
#define SCREEN_H_

#include "types.h"


#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 200
#define SCREEN_SIZE   (SCREEN_WIDTH*SCREEN_HEIGHT)

extern u8 g_sbuff[2][SCREEN_SIZE];
extern u8 g_sbuff_front;

void screen_fill(u8 col);
void screen_swap_buffers(void);
void screen_init(void);

#define screen_set_px(x, y, col) g_sbuff[g_sbuff_front][(y)*SCREEN_WIDTH+(x)] = (col)
//#define screen_set_px(x, y, col) ((u8*)0xA0000)[(y)*SCREEN_WIDTH+(x)] = (col)


#endif // SCREEN_H
