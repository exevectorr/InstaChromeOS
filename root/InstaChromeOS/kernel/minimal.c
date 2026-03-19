// minimal.c
// this was just a test kernel that shows "KERNEL STARTED OK" on the screen. It's not used anymore but I kept it for reference.
// It was used for debugging the kernel under the process of implementing the graphics mode.
// we ran in to a lot of issues with the graphics mode and needed a simple kernel to test the bootloader and multiboot2 loading without the complexity of the graphics code.
// the main error that we couldnt get past after the grub menu. It would just say booting and then fast switch back to the grub menu.
// so this kernel was used to verify that the bootloader and multiboot2 loading was working correctly, and to isolate the graphics code as the source of the issue. Once we confirmed that this minimal kernel worked, we knew that the problem was in the graphics code, which helped us focus our debugging efforts. After fixing the graphics code, we were able to get the full kernel running with graphics support. So this minimal kernel served as an important stepping stone in our development process.
// so if you modify the OS and you run into the same error as our error you can switch to this kernel to verify that the bootloader and multiboot2 loading is working correctly, and to isolate the graphics code as the source of the issue. If this minimal kernel works but the full kernel doesn't, then you know the problem is in the graphics code. If this minimal kernel doesn't work, then you know the problem is in the bootloader or multiboot2 loading. This can save you a lot of time in debugging by quickly narrowing down where the issue is.
// to boot from thi minimal kernel you can use this command in your linux terminal.
// to make the compile this kernel with the boot.s you can use this command in your LINUX terminal:

// cd /mnt/c/users/user/SJT6-45/SIT0-72/InstaChromeOS/root/InstaChromeOS/kernel
// i686-elf-gcc -ffreestanding -c minimal.c -o minimal.o
// i686-elf-gcc -T linker.ld -o minimal.elf minimal.o boot/boot.o -ffreestanding -nostdlib -lgcc

// to boot from the minimal kernel you can use this command in your LINUX terminal: 

// cd /mnt/c/users/user/SJT6-45/SIT0-72/InstaChromeOS/root
// mkdir -p minimal_iso/boot/grub
// cp InstaChromeOS/kernel/minimal.elf minimal_iso/boot/kernel.elf
// cp InstaChromeOS/boot/grub/grub.cfg minimal_iso/boot/grub/
// grub-mkrescue -o minimal.iso minimal_iso
// qemu-system-i386 -cdrom minimal.iso -m 256M -boot order=d

// This kernel is not used in the final OS, but it's kept here for reference and debugging purposes. It can be useful for testing the bootloader and multiboot2 loading without the complexity of the graphics code. If you run into issues with the full kernel, you can switch to this minimal kernel to verify that the bootloader and multiboot2 loading is working correctly, and to isolate the graphics code as the source of the issue.
void kernel_main(void) {
    char* video = (char*)0xB8000;
    const char* msg = "KERNEL STARTED OK";
    int i;
    
    for(i = 0; i < 80*25*2; i+=2) {
        video[i] = ' ';
        video[i+1] = 0x07;
    }
    
    for(i = 0; msg[i]; i++) {
        video[i*2] = msg[i];
        video[i*2+1] = 0x02;
    }
    
    while(1) {
        __asm__ volatile("hlt");
    }
}