.section .multiboot
.align 8

multiboot_header:
    .long 0xE85250D6
    .long 0
    .long 16
    .long -(0xE85250D6 + 0 + 16)
    
    .word 0
    .word 0
    .long 8
header_end:

.section .bss
.align 16
stack_bottom:
    .skip 16384
stack_top:

.section .text
.global _start
_start:
    mov $stack_top, %esp
    push %ebx
    push %eax
    call kernel_main
    cli
.hang:
    hlt
    jmp .hang