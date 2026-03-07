#ifndef PMM_H
#define PMM_H

#include <stdint.h>

/* Physical memory manager */
void pmm_init(void);
void* pmm_alloc_page(void);
void pmm_free_page(void* page);
uint32_t pmm_get_total_memory(void);
uint32_t pmm_get_used_memory(void);
uint32_t pmm_get_free_memory(void);
uint32_t pmm_get_free_pages(void);
uint32_t pmm_get_total_pages(void);

#endif