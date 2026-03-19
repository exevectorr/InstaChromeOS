#include "vga_graphics.h"

static uint8_t* framebuffer = VGA_FRAMEBUFFER;

void vga_init(void) {
    // Switch to Mode 13h using BIOS (requires real mode)
    // For now, we'll assume the bootloader or previous stage set it
    // Clear screen to black
    vga_clear(COLOR_BLACK);
}

void vga_put_pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) return;
    framebuffer[y * VGA_WIDTH + x] = color;
}

uint8_t vga_get_pixel(int x, int y) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) return 0;
    return framebuffer[y * VGA_WIDTH + x];
}

void vga_clear(uint8_t color) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        framebuffer[i] = color;
    }
}

void vga_draw_rect(int x, int y, int width, int height, uint8_t color) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            vga_put_pixel(x + i, y + j, color);
        }
    }
}

void vga_draw_line(int x1, int y1, int x2, int y2, uint8_t color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        vga_put_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}