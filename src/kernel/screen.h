#ifndef SCREEN_H_
#define SCREEN_H_

#include "types.h"


#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 200
#define SCREEN_SIZE   (SCREEN_WIDTH*SCREEN_HEIGHT)

extern u8 _sbuff[2][SCREEN_SIZE];
extern u8 _sbuff_front;

void screen_init(void);
void screen_swap_buffers(void);

#define screen_set_px(x, y, col) _sbuff[_sbuff_front][(y)*SCREEN_WIDTH+(x)] = (col)
//#define screen_set_px(x, y, col) ((u8*)0xA0000)[(y)*SCREEN_WIDTH+(x)] = (col)


#endif // SCREEN_H
