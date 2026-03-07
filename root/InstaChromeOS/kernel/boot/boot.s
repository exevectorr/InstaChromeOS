.section .multiboot
.align 8

.multiboot_header:
    .long 0xE85250D6              /* Magic number */
    .long 0                       /* Architecture 0 (i386) */
    .long 24                       /* Header length */
    .long -(0xE85250D6 + 0 + 24)   /* Checksum */
    
    /* End tag */
    .word 0                        /* Type */
    .word 0                        /* Flags */
    .long 8                         /* Size */

.section .bss
.align 16
stack_bottom:
    .skip 16384                    /* 16KB stack */
stack_top:

.section .text
.global _start
_start:
    /* Set up stack */
    mov $stack_top, %esp
    
    /* Push multiboot info pointer */
    push %ebx
    push %eax
    
    /* Call kernel main */
    call kernel_main
    
    /* Halt if kernel returns */
    cli
.hang:
    hlt
    jmp .hang