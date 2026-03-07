[SYSTEM]
Name=InstaChromeOS
Version=1.0.0
Architecture=i386
Kernel=kernel.elf
Bootloader=GRUB2
Filesystem=FAT32
Memory=128MB
Display=80x25 VGA

[KERNEL]
Type=Monolithic
Scheduler=RoundRobin
TimeSlice=10ms
Syscall_Interface=0x80
GDT_Entries=5
IDT_Entries=256

[DRIVERS]
Console=screen.sys
Keyboard=keyboard.sys
Timer=pit.sys
Disk=ata.sys

[BOOT]
Stage1=boot.s
Stage2=GRUB
Multiboot=Yes
Initrd=No