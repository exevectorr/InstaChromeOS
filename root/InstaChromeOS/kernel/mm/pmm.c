#include "pmm.h"
#include <stddef.h>

/* External symbols from linker script */
extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

/* Physical memory manager variables */
static uint32_t memory_size = 0;
static uint32_t used_memory = 0;
static uint32_t total_pages = 0;
static uint32_t used_pages = 0;
static uint8_t memory_bitmap[32768];  /* 32KB bitmap for up to 128MB memory */

#define PAGE_SIZE 4096
#define BITMAP_SIZE 32768

/* Get total memory in bytes */
uint32_t pmm_get_total_memory(void) {
    return memory_size;
}

/* Get used memory in bytes */
uint32_t pmm_get_used_memory(void) {
    return used_memory;
}

/* Get free memory in bytes */
uint32_t pmm_get_free_memory(void) {
    return memory_size - used_memory;
}

/* Get free pages count */
uint32_t pmm_get_free_pages(void) {
    return total_pages - used_pages;
}

/* Get total pages */
uint32_t pmm_get_total_pages(void) {
    return total_pages;
}

/* Initialize PMM with real memory detection */
void pmm_init(void) {
    /* Simple memory detection - assume 128MB for now */
    /* In a real OS, you'd parse multiboot info here */
    memory_size = 128 * 1024 * 1024;  /* 128MB */
    
    /* Calculate kernel size */
    uint32_t kernel_start = (uint32_t)&_kernel_start;
    uint32_t kernel_end = (uint32_t)&_kernel_end;
    uint32_t kernel_size = kernel_end - kernel_start;
    
    /* Calculate total pages */
    total_pages = memory_size / PAGE_SIZE;
    
    /* Clear bitmap */
    for(uint32_t i = 0; i < BITMAP_SIZE; i++) {
        memory_bitmap[i] = 0;
    }
    
    /* Mark first 1MB as used (BIOS, VGA, etc) */
    for(uint32_t i = 0; i < 256; i++) {  /* First 1MB = 256 pages (4096 * 256 = 1,048,576 bytes) */
        memory_bitmap[i / 8] |= 1 << (i % 8);
        used_pages++;
    }
    
    /* Mark kernel memory as used */
    uint32_t kernel_pages = (kernel_size / PAGE_SIZE) + 1;
    uint32_t kernel_start_page = kernel_start / PAGE_SIZE;
    
    for(uint32_t i = kernel_start_page; i < kernel_start_page + kernel_pages; i++) {
        if(i < total_pages) {
            memory_bitmap[i / 8] |= 1 << (i % 8);
            used_pages++;
        }
    }
    
    /* Mark bitmap memory as used (the bitmap itself occupies memory) */
    uint32_t bitmap_pages = BITMAP_SIZE / PAGE_SIZE + 1;
    uint32_t bitmap_start_page = (uint32_t)memory_bitmap / PAGE_SIZE;
    
    for(uint32_t i = bitmap_start_page; i < bitmap_start_page + bitmap_pages; i++) {
        if(i < total_pages) {
            memory_bitmap[i / 8] |= 1 << (i % 8);
            used_pages++;
        }
    }
    
    /* Update used memory count */
    used_memory = used_pages * PAGE_SIZE;
}

/* Allocate a page */
void* pmm_alloc_page(void) {
    for(uint32_t i = 0; i < total_pages; i++) {
        if(!(memory_bitmap[i / 8] & (1 << (i % 8)))) {
            memory_bitmap[i / 8] |= 1 << (i % 8);
            used_pages++;
            used_memory = used_pages * PAGE_SIZE;
            return (void*)(i * PAGE_SIZE);
        }
    }
    return NULL;  /* Out of memory */
}

/* Free a page */
void pmm_free_page(void* page) {
    if(page == NULL) return;
    
    uint32_t page_num = (uint32_t)page / PAGE_SIZE;
    if(page_num < total_pages) {
        memory_bitmap[page_num / 8] &= ~(1 << (page_num % 8));
        used_pages--;
        used_memory = used_pages * PAGE_SIZE;
    }
}