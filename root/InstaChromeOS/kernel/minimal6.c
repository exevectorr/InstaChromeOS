#include "drivers/vga.h"
#include "drivers/font.h"

void kernel_main(void) {
    // Aktivera grafikläge (helt utan BIOS)
    vga_set_mode(VGA_MODE_GRAPH);
    
    // Rita en gradientbakgrund
    for(int y = 0; y < 200; y++) {
        for(int x = 0; x < 320; x++) {
            vga_put_pixel(x, y, (x + y) % 256);
        }
    }
    
    // Rita en vit rektangel
    vga_draw_rect(50, 50, 100, 50, 0x0F);
    
    // Rita en röd linje
    vga_draw_line(0, 150, 319, 150, 0x04);
    
    // Testa fonten
    draw_string(10, 10, "VGA WORKS!", 0x0F);
    draw_string(10, 25, "NO BIOS!", 0x0A);
    
    while(1) {
        __asm__ volatile("hlt");
    }
}