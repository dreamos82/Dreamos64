# DreamOS64


[![C/C++ CI](https://github.com/dreamos82/Dreamos64/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/dreamos82/Dreamos64/actions/workflows/c-cpp.yml) [![Discord Chat](https://img.shields.io/discord/578193015433330698.svg?style=flat)](https://discordapp.com/channels/578193015433330698/578193713340219392) ![](https://tokei.rs/b1/github/dreamos82/Dreamos64)

DreamOS64 is just my recent attempt to write a kernel again after many years far from osdev, 
But this time in 64bit.

It is not a real replacement of the original DreamOS, and i don't really have any long term plans for it at the moment.

I started this project because i was missing the excitement of low-level programming :) 

## What it does

Actually nothing (more or less)! :D 

It just prints the OS name and few other string. 

But at kernel level several things have been implemented:

* Long mode 
* Kernel load in higher half
* Basic i/o functions (using the VGA bios)
* Basic framebuffer support (it can print strings on the screen, but only on the first half of the screen, the second half will cause a #PF for now) 
* IDT Support
* It can load the kernel using 4kb pages or 2Mb pages
* Added support to access paging data structures with recursion tecnique. 
* Implemented basic physical memory manager
* Enabled paging
* Implement basic kheap memory manager
* Added basic kmalloc, kfree
* Support Acpi V1 and V2
* Local Apic support
* IO-Apic support (Keyboard IRQ enable)
* Suppoort for PSF v1 and v2 fonts in framebuffer mode
* Basic keyboard driver and keyboard map (most of the keys are mapped)
* It prints the Epoch time! :) 
* Extremely basic multi-task/multi-thread support
* Thread switching, thread_sleep and basic spinlock implementation

## Prerequisites: 

These are the packages you need to build and run it: 

* nasm
* qemu and qemu-kvm
* grub-mkrescue
* grub-pc-bin
* xorriso
* mtools
* Gcc cross compiler

## Build

To build dreamos, you need either `gcc` or `clang` supporting the target architecture, a complete guide on how to build the `gcc` cross-compiler, can be found [here](https://github.com/dreamos82/Osdev-Notes/blob/master/99_Appendices/E_Cross_Compilers.md), for DreamOS the target architecture will be `x86-64`.

For `clang` having the toolchain installed should be enough.

### Build the OS
 
Before building the os you need to copy a PsfV2 font in the fonts folder, called default.ps (even if building with framebuffer off) [TODO: expand this part]

A complete guide on how to build _DreamOS 64_ and the parameters availables in the docs folder: [docs/Building.md](docs/Building.md).

To build just run: 

```bash
    make
```

It will create an output file called DreamOs64.iso

## Run

To run just execute: 
```bash
    make run
```

If you use instead:
```bash
   make debug
```

It will output logging information on a logfile.

Finally:

```bash
   make gdb
```

Will compile the OS with debug symbol, and launch qemu with remote debugging enabled (it will wait connection from gdb to start).

There are couple of flags that you can set on the Makefile in order to configure some features: 

* USE_FRAMEBUFFER if set to 1 it will enable framebuffer support (still experimental and probably will not work everywhere since is in very early stage of development), in addition you need a PSF v1 or v2 font file in the location `fonts/default.psf`, if set to 0 (default) it will use the legacy VGA driver.
* SMALL_PAGES if set to 1 it will use 4kb pages (this feature is new, and there are still some problems/unmapped parts that will cause #PF to be generated), if 0 (default) will use 2MB pages. 

### Unit tests

There is a small set of tests implemented using asserts, if you want to run them just run: 

```bash
make tests
```

### Known issues

* If at boot with qemu you get the following error message: 

```
	Booting from DVD/CD...
	Boot failed: Could not read from CDROM (code 0009)
```
This means you are missing the grub-pc-bin package, and you need to install it. 

* If the linking steps fail with several errors message like: 

```
src/kernel/framebuffer/framebuffer.c:122: undefined reference to `_binary_fonts_default_psf_start'
```

This means that the fonts folder is missing (you need either a psf v2 or v1 font file in the fonts folder, the file has to be called default.psf)

### And now show time! :) 

![ezgif com-gif-maker (2)](https://user-images.githubusercontent.com/59960116/163857438-bb5e3ad5-47c5-40d3-9d63-a85381449425.gif)




### Acknowledgements

Well so far a big thank you goes to [DeanoBurrito](https://github.com/DeanoBurrito) that implemented several stuff for Dreamos64 (x2apic, fixed some bugs, logging, and more to come...) not to mention his precious help explaining things and reviewing my crappy code :) 
And his operating system is worth a mention too: [NorthPort](https://github.com/DeanoBurrito/northport)

