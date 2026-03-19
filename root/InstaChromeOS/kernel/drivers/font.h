#ifndef FONT_H
#define FONT_H

#include <stdint.h>

/* 8x8 font */
extern uint8_t font_8x8[144][8];

void draw_char(int x, int y, char c, uint8_t color);
void draw_string(int x, int y, const char* str, uint8_t color);
void draw_number(int x, int y, uint32_t num, uint8_t color);
void draw_hex(int x, int y, uint32_t num, uint8_t color);

#endif