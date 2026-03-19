#ifndef VGA_GRAPHICS_H
#define VGA_GRAPHICS_H

#include <stdint.h>

// VGA Mode 13h (320x200, 256 colors)
#define VGA_WIDTH  320
#define VGA_HEIGHT 200
#define VGA_FRAMEBUFFER ((uint8_t*)0xA0000)

// Colors (standard VGA palette)
#define COLOR_BLACK       0
#define COLOR_BLUE        1
#define COLOR_GREEN       2
#define COLOR_CYAN        3
#define COLOR_RED         4
#define COLOR_MAGENTA     5
#define COLOR_BROWN       6
#define COLOR_LIGHT_GREY  7
#define COLOR_DARK_GREY   8
#define COLOR_LIGHT_BLUE  9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN  11
#define COLOR_LIGHT_RED   12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_YELLOW      14
#define COLOR_WHITE       15

// Initialize VGA graphics mode
void vga_init(void);

// Set a pixel at (x, y) to color
void vga_put_pixel(int x, int y, uint8_t color);

// Get pixel color at (x, y)
uint8_t vga_get_pixel(int x, int y);

// Clear screen with color
void vga_clear(uint8_t color);

// Draw a rectangle
void vga_draw_rect(int x, int y, int width, int height, uint8_t color);

// Draw a line using Bresenham's algorithm
void vga_draw_line(int x1, int y1, int x2, int y2, uint8_t color);

#endif