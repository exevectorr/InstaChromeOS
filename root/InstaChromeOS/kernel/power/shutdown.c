#include "shutdown.h"
#include "../drivers/screen.h"

/* I/O port functions */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

/* QEMU shutdown */
void shutdown_system(void) {
    screen_write("\nSystem shutting down...\n");
    
    /* QEMU shutdown port */
    outw(0x604, 0x2000);
    outw(0x4004, 0x3400); /* Alternative port */
    
    __asm__ volatile ("cli\nhlt");
}

/* Reboot system */
void reboot_system(void) {
    screen_write("\nSystem rebooting...\n");
    
    /* Try keyboard controller reboot */
    uint8_t good;
    do {
        good = inb(0x64);
    } while(good & 0x02);
    outb(0x64, 0xFE);
    
    /* If that fails, triple fault */
    __asm__ volatile ("lidt %0" : : "m" (*(uint16_t*)0));
    __asm__ volatile ("int3");
}