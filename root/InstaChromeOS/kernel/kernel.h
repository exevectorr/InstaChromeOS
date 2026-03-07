#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>

/* Memory information structure */
typedef struct {
    uint32_t total_mb;
    uint32_t used_mb;
    uint32_t free_kb;
} memory_info_t;

/* Function declarations */
void kernel_main(uint32_t magic, uint32_t addr);  // Match the implementation
void init_memory(void);
void init_screen(void);
void init_keyboard(void);
void init_fs(void);
void display_memory_info(void);
void display_welcome(void);
void start_shell(void);

#endif