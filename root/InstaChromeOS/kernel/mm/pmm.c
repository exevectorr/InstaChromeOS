#include "pmm.h"
#include <stddef.h>  /* Add this for NULL definition */

/* Simple bitmap for page allocation */
static uint8_t memory_bitmap[32768];  /* 32KB bitmap for 128MB memory */
static uint32_t total_pages = 0;
static uint32_t used_pages = 0;

#define PAGE_SIZE 4096

/* Initialize PMM */
void pmm_init(void) {
    /* Assume 128MB total memory for simulation */
    total_pages = 128 * 1024 * 1024 / PAGE_SIZE;  /* 128MB / 4KB = 32768 pages */
    
    /* Mark first 16MB as used (kernel and reserved) */
    for(uint32_t i = 0; i < 4096; i++) {
        memory_bitmap[i / 8] |= 1 << (i % 8);
        used_pages++;
    }
}

/* Allocate a page */
void* pmm_alloc_page(void) {
    for(uint32_t i = 0; i < total_pages; i++) {
        if(!(memory_bitmap[i / 8] & (1 << (i % 8)))) {
            memory_bitmap[i / 8] |= 1 << (i % 8);
            used_pages++;
            return (void*)((uint32_t)i * PAGE_SIZE);
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
    }
}

/* Get total memory in bytes */
uint32_t pmm_get_total_memory(void) {
    return total_pages * PAGE_SIZE;
}

/* Get used memory in bytes */
uint32_t pmm_get_used_memory(void) {
    return used_pages * PAGE_SIZE;
}