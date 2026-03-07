/* Linker script for InstaChromeOS kernel */

ENTRY(_start)

SECTIONS
{
    /* Begin at 1MB */
    . = 1M;

    /* Kernel start symbol - use PROVIDE to make it global */
    PROVIDE(_kernel_start = .);

    /* Multiboot header first */
    .multiboot : {
        *(.multiboot)
    }

    /* Code */
    .text : {
        *(.text)
        *(.text.*)
    }

    /* Read-only data */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    /* Data */
    .data : {
        *(.data)
        *(.data.*)
    }

    /* BSS */
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }

    /* Kernel end symbol - use PROVIDE to make it global */
    PROVIDE(_kernel_end = .);

    /* Discard unnecessary sections */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}