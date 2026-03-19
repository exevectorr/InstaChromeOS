#include "vga.h"  /* För outb/inb om det behövs, annars skapa fb.h */
#include "font.h"

/* Framebuffer info */
static uint32_t fb_addr = 0;
static uint32_t fb_width = 1024;
static uint32_t fb_height = 768;
static uint32_t fb_pitch = 0;
static uint8_t fb_bpp = 32;

/* Initiera framebuffer från GRUB */
void fb_init(uint32_t addr, uint32_t width, uint32_t height, uint32_t pitch, uint8_t bpp) {
    fb_addr = addr;
    fb_width = width;
    fb_height = height;
    fb_pitch = pitch;
    fb_bpp = bpp;
}

/* Returnera skärmens bredd */
uint32_t fb_get_width(void) {
    return fb_width;
}

/* Returnera skärmens höjd */
uint32_t fb_get_height(void) {
    return fb_height;
}

/* Sätt en pixel med RGB-färg (32 bpp) */
void fb_put_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if(x < 0 || x >= fb_width || y < 0 || y >= fb_height || !fb_addr) return;
    
    uint32_t* fb = (uint32_t*)fb_addr;
    uint32_t color = (r << 16) | (g << 8) | b;
    fb[y * (fb_pitch/4) + x] = color;
}

/* Sätt en pixel med 16-bitars färg (565 RGB) */
void fb_put_pixel_16(int x, int y, uint16_t color) {
    if(x < 0 || x >= fb_width || y < 0 || y >= fb_height || !fb_addr) return;
    
    uint16_t* fb = (uint16_t*)fb_addr;
    fb[y * (fb_pitch/2) + x] = color;
}

/* Rensa hela skärmen med en RGB-färg */
void fb_clear_screen(uint8_t r, uint8_t g, uint8_t b) {
    if(!fb_addr) return;
    
    uint32_t color = (r << 16) | (g << 8) | b;
    uint32_t* fb = (uint32_t*)fb_addr;
    
    for(int y = 0; y < fb_height; y++) {
        for(int x = 0; x < fb_width; x++) {
            fb[y * (fb_pitch/4) + x] = color;
        }
    }
}

/* Rita en horisontell linje */
void fb_draw_hline(int y, int x1, int x2, uint8_t r, uint8_t g, uint8_t b) {
    if(y < 0 || y >= fb_height) return;
    if(x1 < 0) x1 = 0;
    if(x2 >= fb_width) x2 = fb_width - 1;
    
    uint32_t color = (r << 16) | (g << 8) | b;
    uint32_t* fb = (uint32_t*)fb_addr;
    
    for(int x = x1; x <= x2; x++) {
        fb[y * (fb_pitch/4) + x] = color;
    }
}

/* Rita en vertikal linje */
void fb_draw_vline(int x, int y1, int y2, uint8_t r, uint8_t g, uint8_t b) {
    if(x < 0 || x >= fb_width) return;
    if(y1 < 0) y1 = 0;
    if(y2 >= fb_height) y2 = fb_height - 1;
    
    uint32_t color = (r << 16) | (g << 8) | b;
    uint32_t* fb = (uint32_t*)fb_addr;
    
    for(int y = y1; y <= y2; y++) {
        fb[y * (fb_pitch/4) + x] = color;
    }
}

/* Rita en fylld rektangel */
void fb_draw_rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
    for(int j = 0; j < h; j++) {
        for(int i = 0; i < w; i++) {
            fb_put_pixel(x + i, y + j, r, g, b);
        }
    }
}

/* Rita en ram (ej fylld) */
void fb_draw_frame(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
    fb_draw_hline(y, x, x + w - 1, r, g, b);
    fb_draw_hline(y + h - 1, x, x + w - 1, r, g, b);
    fb_draw_vline(x, y, y + h - 1, r, g, b);
    fb_draw_vline(x + w - 1, y, y + h - 1, r, g, b);
}