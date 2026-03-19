#include "vga.h"

/* PORTS */
#define VGA_MISC_WRITE  0x3C2
#define VGA_SEQ_INDEX   0x3C4
#define VGA_SEQ_DATA    0x3C5
#define VGA_CRTC_INDEX  0x3D4
#define VGA_CRTC_DATA   0x3D5
#define VGA_GC_INDEX    0x3CE
#define VGA_GC_DATA     0x3CF
#define VGA_AC_INDEX    0x3C0
#define VGA_AC_WRITE    0x3C0
#define VGA_INSTAT_READ 0x3DA

/* OBS: outb och inb finns i vga.h – ta INTE med dem här! */

/* Mode 13h register dump */
static uint8_t g_320x200x256[] = {
    /* MISC */
    0x63,
    /* SEQ */
    0x03, 0x01, 0x0F, 0x00, 0x0E,
    /* CRTC */
    0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
    0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
    0xFF,
    /* GC */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
    0xFF,
    /* AC */
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x01, 0x00, 0x0F, 0x00, 0x00
};

void vga_set_mode_13h(void) {
    uint8_t* regs = g_320x200x256;

    /* MISC */
    outb(VGA_MISC_WRITE, *regs++);
    
    /* SEQ */
    for (int i = 0; i < 5; i++) {
        outb(VGA_SEQ_INDEX, i);
        outb(VGA_SEQ_DATA, *regs++);
    }

    /* Unlock CRTC */
    outb(VGA_CRTC_INDEX, 0x03);
    outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) | 0x80);
    outb(VGA_CRTC_INDEX, 0x11);
    outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) & ~0x80);

    /* CRTC */
    for (int i = 0; i < 25; i++) {
        outb(VGA_CRTC_INDEX, i);
        outb(VGA_CRTC_DATA, *regs++);
    }

    /* GC */
    for (int i = 0; i < 9; i++) {
        outb(VGA_GC_INDEX, i);
        outb(VGA_GC_DATA, *regs++);
    }

    /* AC */
    for (int i = 0; i < 21; i++) {
        inb(VGA_INSTAT_READ);
        outb(VGA_AC_INDEX, i);
        outb(VGA_AC_WRITE, *regs++);
    }

    /* Enable display */
    inb(VGA_INSTAT_READ);
    outb(VGA_AC_INDEX, 0x20);
}

/* Framebuffer */
static uint8_t* fb = (uint8_t*)0xA0000;

void vga_put_pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) return;
    fb[y * VGA_WIDTH + x] = color;
}

void vga_clear_screen(uint8_t color) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        fb[i] = color;
    }
}