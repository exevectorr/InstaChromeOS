// minimal2.c
// if you ran into the same error as us and tried the minimal.c kernel and it worked but the full kernel doesn't, then you can try this minimal2.c kernel. It's similar to the minimal.c kernel but it uses the graphics mode instead of the text mode. It will draw a simple gradient on the screen to verify that the graphics mode is working correctly. If this minimal2.c kernel works but the full kernel doesn't, then you know the problem is in the graphics code. If this minimal2.c kernel doesn't work, then you know the problem is in the bootloader or multiboot2 loading. This can save you a lot of time in debugging by quickly narrowing down where the issue is.
// this is nkept in the OS for reference and debugging purposes. It's not used in the final OS, but it can be useful for testing the bootloader and multiboot2 loading without the complexity of the graphics code. If you run into issues with the full kernel, you can switch to this minimal2.c kernel to verify that the bootloader and multiboot2 loading is working correctly, and to isolate the graphics code as the source of the issue. If this minimal2.c kernel works but the full kernel doesn't, then you know the problem is in the graphics code. If this minimal2.c kernel doesn't work, then you know the problem is in the bootloader or multiboot2 loading. This can save you a lot of time in debugging by quickly narrowing down where the issue is.
// minimal3.c - Ingen VGA, bara textläge
void kernel_main(void) {
    // Skriv direkt till VGA-textminnet (0xB8000)
    char* video = (char*)0xB8000;
    const char* msg = "INSTACHROMEOS";
    int i;
    
    // Rensa skärmen
    for(i = 0; i < 80*25*2; i+=2) {
        video[i] = ' ';
        video[i+1] = 0x07;
    }
    
    // Skriv meddelande
    for(i = 0; msg[i]; i++) {
        video[i*2] = msg[i];
        video[i*2+1] = 0x02; // Grön text
    }
    
    while(1) {
        __asm__ volatile("hlt");
    }
}