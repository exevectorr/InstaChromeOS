#ifndef FB_H
#define FB_H

#include <stdint.h>

void fb_init(uint32_t addr, uint32_t width, uint32_t height, uint32_t pitch, uint8_t bpp);
void fb_put_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
void fb_clear_screen(uint8_t r, uint8_t g, uint8_t b);
uint32_t fb_get_width(void);
uint32_t fb_get_height(void);

#endif