/* Läs multiboot info för framebuffer */
struct multiboot_tag_framebuffer {
    uint32_t type;
    uint32_t size;
    uint64_t fb_addr;
    uint32_t fb_pitch;
    uint32_t fb_width;
    uint32_t fb_height;
    uint8_t fb_bpp;
    uint8_t fb_type;
    uint8_t reserved;
} __attribute__((packed));

void kernel_main(uint32_t magic, uint32_t addr) {
    /* Kolla att vi har Multiboot2 */
    if(magic != 0x36d76289) return;
    
    /* Hitta framebuffer-taggen */
    uint32_t* tags = (uint32_t*)(addr + 8);
    uint32_t fb_addr = 0;
    uint32_t fb_width = 1024;
    uint32_t fb_height = 768;
    uint32_t fb_pitch = 0;
    
    while(*tags != 0) {
        if(*tags == 8) {  /* Framebuffer tag */
            struct multiboot_tag_framebuffer* fb_tag = (struct multiboot_tag_framebuffer*)tags;
            fb_addr = (uint32_t)fb_tag->fb_addr;
            fb_pitch = fb_tag->fb_pitch;
            fb_width = fb_tag->fb_width;
            fb_height = fb_tag->fb_height;
            break;
        }
        uint32_t size = *(tags + 1);
        tags = (uint32_t*)((uint32_t)tags + size);
    }
    
    /* Om GRUB inte satte något, använd standard */
    if(!fb_addr) {
        fb_addr = 0xFD000000;
        fb_pitch = 1024 * 4;
    }
    
    /* Fyll skärmen med blått */
    uint32_t* fb = (uint32_t*)fb_addr;
    for(int y = 0; y < fb_height; y++) {
        for(int x = 0; x < fb_width; x++) {
            fb[y * (fb_pitch/4) + x] = 0x0000FF; /* Blått */
        }
    }
    
    /* Rita en vit rektangel */
    for(int y = 100; y < 200; y++) {
        for(int x = 100; x < 300; x++) {
            fb[y * (fb_pitch/4) + x] = 0xFFFFFF; /* Vitt */
        }
    }
    
    while(1) {
        __asm__ volatile("hlt");
    }
}