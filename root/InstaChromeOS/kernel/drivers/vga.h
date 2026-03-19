#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define VGA_WIDTH  320
#define VGA_HEIGHT 200

/* I/O-funktioner */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* VGA-funktioner */
void vga_set_mode_13h(void);
void vga_put_pixel(int x, int y, uint8_t color);
void vga_clear_screen(uint8_t color);
void vga_draw_rect(int x, int y, int w, int h, uint8_t color);

#endif