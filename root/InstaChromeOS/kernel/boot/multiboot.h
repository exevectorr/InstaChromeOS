/* multiboot.h - Multiboot header definition */
#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#define MULTIBOOT2_HEADER_MAGIC 0xE85250D6
#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289

/* Multiboot 2 header structure */
struct multiboot_header {
    uint32_t magic;
    uint32_t architecture;
    uint32_t header_length;
    uint32_t checksum;
};

/* Memory map entry structure */
struct multiboot_mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed));

/* Memory types */
#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5

#endif