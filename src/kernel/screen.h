#ifndef SCREEN_H_
#define SCREEN_H_

#include "types.h"


#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 200
#define SCREEN_SIZE   (SCREEN_WIDTH * SCREEN_HEIGHT)

extern u8 g_sbuff[2][SCREEN_SIZE];
extern u8 g_sbuff_front;

void screen_draw_bitmap(const u16* buff, u16 x, u16 y, u16 width, u16 height);
void screen_draw_bitmap_mono(const u8* buff, u16 x, u16 y, u16 width, u16 height);
void screen_draw_line(i32 x1, i32 y1, i32 x2, i32 y2, u8 col);
void screen_fill(u8 col);
void screen_swap_buffers(void);
void screen_init(void);

#define screen_set_px(x, y, col) g_sbuff[g_sbuff_front][(y) * (SCREEN_WIDTH) + (x)] = (col)

#endif // SCREEN_H
