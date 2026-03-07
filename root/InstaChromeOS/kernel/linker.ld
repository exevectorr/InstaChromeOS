ENTRY(_start)

SECTIONS
{
    /* Begin at 1MB */
    . = 1M;

    /* Multiboot header first */
    .multiboot : {
        *(.multiboot)
    }

    /* Code */
    .text : {
        *(.text)
    }

    /* Read-only data */
    .rodata : {
        *(.rodata)
    }

    /* Data */
    .data : {
        *(.data)
    }

    /* BSS */
    .bss : {
        *(.bss)
    }
}