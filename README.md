# InstaChromeOS

A lightweight, educational operating system built from scratch for x86 architecture by [exevectorr](https://github.com/exevectorr). Features a custom kernel, FAT32 filesystem, BIOS-like interface, and a fully functional shell with file management capabilities.

---

## 📋 Table of Contents
- [Core Architecture](#-core-architecture)
- [Boot Process Deep Dive](#-boot-process-deep-dive)
- [Memory Management Explained](#-memory-management-explained)
- [Filesystem Architecture](#-filesystem-architecture)
- [Interrupt Handling System](#-interrupt-handling-system)
- [Device Drivers](#-device-drivers)
- [Power Management](#-power-management)
- [Shell and User Interface](#-shell-and-user-interface)
- [User Guide (v1.1.9)](#-user-guide-v119)

---

## 🏗️ Core Architecture

### Design Philosophy
InstaChromeOS is built as a **monolithic kernel**, which means all core services run in the same address space. This design was chosen for its simplicity and performance, making it ideal for an educational operating system where understanding interactions between components is key. Unlike microkernels that run services in separate spaces, monolithic kernels have all components tightly integrated, which means faster communication but also means a bug in one component can crash the entire system.

### The Layered Architecture
The kernel is organized into distinct layers, each with a specific responsibility:

**Hardware Layer**
This is the physical computer – the CPU, RAM, disk, keyboard, mouse, and screen. The kernel cannot talk to these directly without going through the next layer.

**Hardware Abstraction Layer (HAL)**
The HAL consists of device drivers that know how to communicate with specific hardware components. For example, the screen driver knows that writing to memory address 0xB8000 will appear on the screen. The keyboard driver knows that reading from port 0x60 gives you the last key pressed. This layer hides hardware details from the rest of the kernel.

**Core Kernel Services**
This is where the real operating system logic lives:
- **Memory Manager**: Keeps track of which parts of RAM are free and which are used
- **Filesystem**: Organizes data into files and directories on disk
- **Process Manager**: Currently handles only one process (the shell), but designed for expansion
- **Interrupt Manager**: Routes hardware events to the right handlers

**System Call Interface**
This layer provides a controlled way for user programs to request kernel services. Currently, the shell calls kernel functions directly, but this layer is designed to eventually provide protection between user programs and the kernel.

**User Applications**
The shell is currently the only user application, but the architecture is designed to support more.

### How Layers Communicate
Communication between layers happens through well-defined function calls. For example:

1. The shell needs to display text – it calls `screen_write()`
2. `screen_write()` is in the HAL – it knows how to put characters in video memory
3. It writes directly to address 0xB8000, which the hardware displays

There are no complex message passing systems or IPC (Inter-Process Communication) – just function calls. This simplicity is intentional for an educational OS.

---

## 🔄 Boot Process Deep Dive

### What Happens from Power-On to Shell

**Phase 1: CPU Initialization**
When power is applied, the CPU starts in real mode, which is a 16-bit compatibility mode that behaves like the original 8086 processor. The CPU begins executing at address 0xFFFFFFF0 (for modern CPUs) or 0xFFFF0 (for older ones), which is mapped to ROM containing the BIOS.

**Phase 2: BIOS/UEFI**
The BIOS (Basic Input/Output System) performs Power-On Self Test (POST) to check that essential hardware is working – CPU, RAM, disk controllers. It then initializes basic hardware and looks for a boot device according to the configured boot order. When it finds a bootable device with a valid boot sector, it loads the first stage bootloader into memory at address 0x7C00 and jumps to it.

**Phase 3: GRUB Stage 1**
The first stage of GRUB is tiny – it fits in the 512-byte boot sector. Its only job is to load the next stage from disk, because 512 bytes isn't enough to do anything complex. It contains just enough code to read the disk and load stage 1.5 or stage 2.

**Phase 4: GRUB Stage 2**
This is the full GRUB you interact with – the menu, command line, and all the features. Stage 2 understands filesystems, so it can load your kernel from a file. When you select InstaChromeOS:

1. GRUB reads the kernel file `kernel.elf` from disk
2. It parses the ELF headers to find out where each section should be loaded
3. It loads the sections into memory at the addresses specified in the linker script
4. It looks for a Multiboot2 header to verify this is a Multiboot2-compliant kernel
5. It sets up a minimal environment and jumps to the entry point `_start`

**Phase 5: Assembly Stub**
The first kernel code to run is `_start` in `boot.s`. This is written in assembly because C needs a stack and initialized global variables, which aren't set up yet. The stub:

1. Sets up a stack by loading `$stack_top` into `%esp`. The stack is essential for C functions to store local variables and return addresses.
2. Pushes the information GRUB passed – the magic number in `%eax` and the multiboot info structure address in `%ebx` – onto the stack so `kernel_main` can access them
3. Calls `kernel_main`
4. If `kernel_main` ever returns, it halts the CPU

**Phase 6: C Kernel Initialization**
Now we're in C code, which is much more expressive. `kernel_main` initializes everything in careful order:

**Step 1: Screen First**
`init_screen()` clears the screen and sets up the cursor position. The screen is initialized first so that any errors during later initialization can be displayed.

**Step 2: Memory Manager**
`pmm_init()` sets up the physical memory manager. It calculates how much RAM is available, creates a bitmap to track used pages, and marks the memory used by the kernel as allocated so nothing else overwrites it.

**Step 3: Interrupts**
`idt_init()` sets up the Interrupt Descriptor Table. This is critical because without interrupts, the CPU would never know when a key is pressed. The IDT tells the CPU what function to call for each possible interrupt.

**Step 4: Input Devices**
`init_keyboard()` and `mouse_init()` initialize the input devices. They clear state and install interrupt handlers so that when keys are pressed or the mouse moves, the right functions are called.

**Step 5: Filesystem**
`init_fs()` creates the initial filesystem structure in memory – the root directory, InstaChromeOS folder, system folder, and all the default files.

**Phase 7: Boot Screens**
With everything initialized, the kernel shows three screens:

**BIOS Screen**: A retro-style BIOS screen that displays for 2.5 seconds. This gives the system a classic feel and shows system information like the build date.

**Memory Information**: Shows real memory statistics for 5 seconds – total memory, used memory, free memory, and memory usage percentage. This is calculated from the PMM data.

**Welcome Screen**: Finally, the welcome screen appears with the shell prompt, ready for user input.

**Phase 8: Shell**
`start_shell()` enters an infinite loop that:
1. Waits for a character from the keyboard
2. When Enter is pressed, processes the command
3. Executes the appropriate function
4. Prints a new prompt

The system is now fully operational.

---

## 📝 Memory Management Explained

### Physical Memory Layout
When InstaChromeOS starts with 128MB of RAM, that memory is a continuous range of addresses from 0 to 134,217,727 (128 × 1024 × 1024). But not all of it is available for the kernel:

**0x00000000 – 0x0009FFFF (640KB)**
This area is called conventional memory. It contains the real mode Interrupt Vector Table (IVT) and BIOS Data Area (BDA). In real mode, this is where critical system information lives. The kernel never uses this area.

**0x000A0000 – 0x000FFFFF (384KB)**
This is the upper memory area, reserved for hardware:
- 0xA0000 – 0xBFFFF: VGA video memory
- 0xC0000 – 0xC7FFF: Video ROM
- 0xF0000 – 0xFFFFF: System BIOS

The screen driver writes to 0xB8000 specifically – this is where text mode VGA displays characters.

**0x00100000 – 0x07FFFFFF (127MB)**
This is where the kernel lives and where all free memory is. The kernel is loaded at 0x100000 (1MB) and everything above that is available for allocation.

### The Bitmap Allocator
The PMM uses a **bitmap allocator** to track which pages are free. Each page is 4KB, and each bit in the bitmap represents one page:

- Bit = 0 → page is free
- Bit = 1 → page is used

For 128MB of RAM, there are 32,768 pages (128MB / 4KB). The bitmap needs 32,768 bits, which is 4,096 bytes (32,768 / 8). This is tiny – less than one page itself.

### How Allocation Works
When a component needs memory, it calls `pmm_alloc_page()`:

1. The allocator scans the bitmap from the beginning
2. It looks for a bit that is 0 (free page)
3. When it finds one, it sets that bit to 1 (marks as used)
4. It calculates the address: `page_number * 4096`
5. It returns that address

This is a first-fit algorithm – simple but effective for a small system.

### What the Kernel Reserves
During initialization, the PMM marks several areas as used:

**First 1MB (0x00000000 – 0x000FFFFF)**
All 256 pages in the first 1MB are marked used because they contain BIOS, video memory, and other reserved areas.

**Kernel Memory**
The kernel occupies a continuous range from its start address (1MB) to its end address. All pages containing kernel code and data are marked used.

**Bitmap Memory**
The bitmap itself occupies memory – it's stored in a static array that's part of the kernel's data section. These pages are also marked used.

### Memory Statistics
The kernel can report detailed memory statistics:

**Total Memory**: 128MB (32768 pages) – this is what the system detected
**Used Memory**: Pages marked as used × 4KB – includes kernel + reserved + bitmap
**Free Memory**: Total − Used – what's available for allocation
**Memory Usage**: (Used × 100) ÷ Total – a percentage

These statistics are displayed during boot and can be used to verify the memory manager is working correctly.

---

## 💾 Filesystem Architecture

### The Two-Layer Design
InstaChromeOS has a unique two-layer filesystem design:

**RAM Filesystem (`fat32.c`)**
This is a filesystem that exists entirely in memory. When you create files with `tayn`, they're stored in RAM and disappear when you shut down. This is perfect for testing and temporary files because it's fast and doesn't require disk access.

**Disk Filesystem (`fat32_disk.c`)**
This filesystem reads and writes to an actual disk (or disk image). It's more complex because it must handle sector I/O, FAT tables, and clusters. Files created here persist across reboots.

The two layers share the same interface, so the shell doesn't need to know which one it's using – it just calls functions like `fs_create_file()` and the appropriate implementation handles it.

### The Filesystem Tree
At initialization, the RAM filesystem creates a standard tree:
```
Root/
└── InstaChromeOS/
          ├── system/
          │     ├── int_handler/
          │     │        └── int.ih
          │     ├── OSsys.os
          │     ├── Linker.hmk
          │     └── instachrome.jl
          ├── personal/
          ├── space/
          └── tmp/
```


**Root**: The top-level directory
**InstaChromeOS**: Main system directory
**system**: Contains system configuration files
**int_handler**: Interrupt handler configuration
**personal**: For user files
**space**: Workspace for temporary projects
**tmp**: Temporary files that may be cleared on reboot

### How Files Are Represented
In memory, each file or directory is a node in a tree:

**name**: The filename (up to 255 characters)
**is_directory**: A flag indicating whether this is a directory
**parent**: Pointer to the parent directory
**children**: Array of pointers to child nodes (for directories)
**content**: The file's data (for files)
**content_size**: How much data is stored

This structure makes navigation easy – to change to a directory, the shell just follows pointers. To list a directory, it iterates through the children array.

### Directory Operations
**cd (Change Directory)**
When you type `cd personal`, the shell calls `fs_change_dir("personal")`. This function looks in the current directory's children for a directory named "personal". If found, it updates the current directory pointer.

**lst (List Directory)**
`fs_list_current()` iterates through the current directory's children and prints each one with `[DIR]` or `[FILE]` prefix.

### File Operations
**tayn (Create File)**
`fs_create_file()` checks if a file with the same name already exists, then creates a new node with `is_directory = 0` and adds it to the current directory's children.

**run -v (View File)**
`fs_display_file()` finds the file node, then prints its content character by character.

**load -s (Edit File)**
This is the most complex operation. It:
1. Reads existing content (if any)
2. Displays it for reference
3. Enters a line-by-line editor
4. When F2 is pressed, writes the new content to the node

### The Editor
The built-in editor is simple but functional:
- Each line starts with `> `
- Backspace works
- F2 saves and exits
- ESC cancels without saving

Content is stored in the node's `content` array, which has a fixed size of 4096 bytes – enough for several screens of text.

---

## ⚡ Interrupt Handling System

### What Are Interrupts?
Interrupts are signals from hardware that tell the CPU to stop what it's doing and handle an event. When you press a key, the keyboard controller sends an interrupt. When the timer ticks, it sends an interrupt. Without interrupts, the CPU would have to constantly check for input (polling), which wastes power and CPU time.

### The Interrupt Descriptor Table
The IDT is a table that tells the CPU what function to call for each interrupt. There are 256 possible interrupts:

**0-31**: CPU exceptions (divide by zero, page fault, etc.)
**32-47**: Hardware interrupts (IRQs)
**48-255**: Software interrupts (system calls)

Each entry in the IDT contains:
- **Address of the handler function** (split into low and high parts)
- **Selector** (which code segment to use)
- **Flags** (privilege level, type, present bit)

### Setting Up the IDT
When `idt_init()` runs, it:

1. Creates an empty IDT with 256 entries
2. Remaps the PIC (Programmable Interrupt Controller) so IRQs don't conflict with CPU exceptions
3. Sets entries for IRQ0 (timer), IRQ1 (keyboard), and IRQ12 (mouse)
4. Loads the IDT with the `lidt` instruction
5. Enables interrupts with `sti`

### How Interrupts Are Handled
When a key is pressed:

1. The keyboard controller sends IRQ1 to the PIC
2. The PIC interrupts the CPU with vector 33 (IRQ1 + 32)
3. The CPU saves its state and looks up vector 33 in the IDT
4. It jumps to `irq1_handler` (assembly stub)
5. `irq1_handler` pushes the IRQ number and calls `irq_common_stub`
6. `irq_common_stub` saves all registers and calls `irq_handler`
7. `irq_handler` calls the registered handler (from `irq_routines[1]`)
8. The keyboard handler reads the scan code from port 0x60
9. It translates the scan code to a character and puts it in the keyboard buffer
10. It sends EOI (End of Interrupt) to the PIC
11. The CPU restores its state and returns to what it was doing

### The Keyboard Buffer
The keyboard buffer is a circular queue that holds characters until the shell reads them:

**buffer_head**: Points to where the next character will be written
**buffer_tail**: Points to the next character to read

When an interrupt handler adds a character, it increments head. When the shell reads a character, it increments tail. If head catches up to tail, the buffer is full and new characters are dropped.

### Why This Matters
This interrupt-driven design means the CPU can be doing other things (like running the shell) while waiting for input. When a key is pressed, the CPU briefly handles it and then returns to what it was doing. This is much more efficient than constantly checking for keys.

---

## 🖱️ Device Drivers

### Screen Driver
The screen driver writes directly to VGA text mode memory at address 0xB8000. This memory is organized as 80 columns × 25 rows, with each character taking 2 bytes:

**Byte 0**: ASCII character
**Byte 1**: Color attribute (4 bits foreground, 4 bits background)

Colors are defined by combining foreground and background:

0: Black
1: Blue
2: Green
3: Cyan
4: Red
5: Magenta
6: Brown
7: Light Grey
8: Dark Grey
9: Light Blue
10: Light Green
11: Light Cyan
12: Light Red
13: Light Magenta
14: Light Brown
15: White

The driver maintains a cursor position and handles scrolling when the screen fills up.

### Keyboard Driver
The keyboard driver reads scan codes from port 0x60 and translates them to ASCII characters. Scan codes are different from ASCII – for example, the 'A' key sends scan code 0x1E, not the ASCII value 65.

The driver handles:
- **Shift**: Modifies the mapping to uppercase
- **Caps Lock**: Toggles case
- **Ctrl/Alt**: Currently not used but recognized
- **F-keys**: Translated to special values (KEY_F1, KEY_F2, etc.)

Two keyboard layouts are supported:
- **US English**: Standard QWERTY layout
- **Swedish**: QWERTY with å, ä, ö on special keys

The layout can be switched at runtime with the `KeySifh` command.

### Mouse Driver
The mouse driver reads 3-byte packets from the PS/2 port:

**Byte 1**: Button states and overflow flags
**Byte 2**: X movement (signed)
**Byte 3**: Y movement (signed)

The driver tracks:
- **Position**: Updated with each movement
- **Buttons**: Left, right, middle states
- **Cursor visibility**: Can be turned on/off

The cursor is drawn by XOR-ing pixels onto the screen. This is done by reading the current character at the cursor position, inverting its color, and writing it back.

### ATA Disk Driver
The ATA driver reads and writes sectors from IDE disks using PIO (Programmed I/O) mode:

**Read**: Send read command, wait for DRQ, read 256 words (512 bytes)
**Write**: Send write command, wait for DRQ, write 256 words, flush cache

The driver supports:
- LBA addressing (Logical Block Addressing)
- Multiple sector reads/writes
- Disk identification
- Cache flushing

---

## 🔋 Power Management

### Shutdown
The `shutdown_system()` function tries multiple methods to power off:

**Method 1: ACPI**
It scans memory for the ACPI RSDP (Root System Description Pointer). If found, it attempts to use the \_S5 object to power off.

**Method 2: QEMU**
For QEMU emulation, it writes 0x2000 to port 0x604, which triggers a shutdown.

**Method 3: BIOS**
It attempts to use APM (Advanced Power Management) via INT 15.

**Method 4: Halt**
If all else fails, it disables interrupts and halts the CPU.

### Reboot
`reboot_system()` also tries multiple methods:

**Method 1: Keyboard Controller**
It pulses the reset line by writing 0xFE to port 0x64.

**Method 2: Triple Fault**
It loads a zero-length IDT and triggers an interrupt, causing a triple fault which resets the CPU.

---

## 🖥️ Shell and User Interface

### Command Processing
The shell runs in an infinite loop:

1. Wait for a character from the keyboard buffer
2. If it's Enter, process the command:
   - Copy the command string (characters collected)
   - Compare against known commands with `strcmp()`
   - Execute the appropriate function
   - Print a new prompt
3. If it's Backspace, remove last character
4. Otherwise, add character to command buffer and echo it

### Command Parsing
Commands are parsed by checking prefixes:
- `cd `: Look for "cd " at the start (3 characters)
- `tayn `: Look for "tayn " (5 characters)
- `mkdir `: Look for "mkdir " (6 characters)

The remaining part (after the space) is passed as an argument.

### The Editor
When `load -s` is called, the shell enters editing mode:
1. If the file exists, its content is displayed
2. Each new line starts with `> `
3. Backspace works
4. F2 saves the content and returns to shell
5. ESC cancels without saving

The editor is line-based – you can't move the cursor up to previous lines, but you can edit the current line with backspace.

---

## 🚀 Version 1.2.0 Roadmap

### Current Status (v1.1.9)
- ✅ Stable monolithic kernel
- ✅ Physical memory manager
- ✅ RAM-based FAT32 filesystem
- ✅ Keyboard with US/Swedish layouts
- ✅ VGA text mode display
- ✅ Basic shell with 10+ commands
- ✅ File editor with F2 save
- ✅ Power management (shutdown/reboot)

### Coming in v1.2.0: Graphics Mode

#### Graphics Infrastructure
The biggest change in 1.2.0 is moving from text mode to VESA/VBE graphics. This requires:

**Framebuffer Support**
Instead of writing to 0xB8000, the kernel will need to manage a linear framebuffer – a large block of memory where each pixel is represented by 1-4 bytes. The screen resolution will increase from 80×25 characters to at least 640×480 pixels.

**VESA BIOS Extensions**
To switch to graphics mode, the kernel must use VESA BIOS Extensions (VBE) to query available modes and set the desired mode. This involves calling BIOS interrupts, which requires switching to real mode temporarily.

**Font Rendering**
With characters no longer built into hardware, the kernel will need to include bitmap fonts and code to draw characters pixel by pixel. This means implementing a console that renders text to the framebuffer.

#### Mouse in Graphics Mode
The mouse cursor will change from a simple text cursor to a graphical pointer. This requires:

- Storing a cursor bitmap
- Drawing the cursor without disturbing the background
- Saving the background before drawing the cursor
- Restoring it when the cursor moves

#### Windowing System
With graphics comes the possibility of a simple windowing system:

**Window Manager**
A basic window manager that can create, move, and close windows. Each window would have its own framebuffer for drawing.

**Widgets**
Simple GUI elements like buttons, text boxes, and menus. These would respond to mouse clicks and keyboard input.

**Desktop**
A background image or color with icons that launch applications.

#### Applications
New applications that take advantage of graphics:

**Graphics Editor**
A simple paint program that lets users draw with the mouse, change colors, and save images.

**File Manager GUI**
Instead of typing `lst`, users could click on folders to navigate.

**System Monitor**
Real-time graphs of memory usage, CPU load (when implemented), and disk activity.

**Games**
Simple games like Snake, Tetris, or Minesweeper that run in graphics mode.

#### Technical Challenges

**Memory Management for Framebuffer**
The framebuffer could be several megabytes – at 1024×768 with 32-bit color, that's 3MB just for the screen. The memory manager must handle this allocation.

**Double Buffering**
To prevent flicker, the system needs two buffers – one to draw into and one to display. When drawing is complete, the buffers are swapped.

**Input Handling**
Mouse events become more important in graphics mode – clicks need to be routed to the correct window based on cursor position.

**Performance**
Drawing graphics is much more CPU-intensive than text mode. Optimizations will be needed to keep the system responsive.

---

## 📝 License

This project is licensed under the Apache 2.0 License - see the LICENSE file for details.

---

*Built with ❤️ by [exevectorr](https://github.com/exevectorr) for learning and exploration*
