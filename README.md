# DreamOS64


[![C/C++ CI](https://github.com/dreamos82/Dreamos64/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/dreamos82/Dreamos64/actions/workflows/c-cpp.yml) [![Discord Chat](https://img.shields.io/discord/578193015433330698.svg?style=flat)](https://discordapp.com/channels/578193015433330698/578193713340219392) ![](https://tokei.rs/b1/github/dreamos82/Dreamos64)

DreamOS64 is just my recent attempt to write a kernel again after many years far from osdev,
But this time in 64bit.

It is not a real replacement of the original DreamOS, and i don't really have any long term plans for it at the moment.

I started this project because i was missing the excitement of low-level programming :)

## What it does

Actually not much! :D

It just prints the OS name, a logo and few other string.

But at kernel level several things have been implemented:

* Long mode
* Kernel load in higher half
* Basic I/O functions (using the VGA bios)
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
* Basic Virtual Memory implementation
* Initial Userspace support (so far can run only an idle userspace thread)
* Basic syscall mechanism (altough no real syscalls are implemented, just one that prints the string "example")

## Prerequisites:

These are the packages required to build and run it:

* nasm
* qemu and qemu-kvm
* grub-mkrescue
* grub-pc-bin
* xorriso
* mtools
* Gcc cross compiler or Clang

To build _Dreamos64_, a toolchain that supports the target architecture is required,  it can be either   `gcc`  (in our case `x86-64`) or `clang` installed.

A complete guide on how to build the `gcc` cross-compiler, can be found [here](https://github.com/dreamos82/Osdev-Notes/blob/master/99_Appendices/E_Cross_Compilers.md), for `DreamOS64` the target architecture will be `x86-64`.

For `clang` we just need to install the `llvm`and the `lld` packages (the exact package name depends on the distribution used).

## Compiling and Running the OS

For a  complete guide on how to build _DreamOS64_ and the parameters that are availables is in the docs folder: [docs/Building.md](docs/Building.md).

### Build the OS

Before building the os we need to copy a PSF font (either v1 or v2) in the fonts folder, and change its name to `default.psf` (even if we are building with framebuffer off).

Once all the parameters in `build/Config.mk` are set, to build just type:

```bash
    make
```

It will use the default goal `build` and produce a bootable iso in the `dist/` folder called.

## Run and Debug

To launch the OS in qemu just use:

```bash
    make run
```

Instead if we type:

```bash
   make debug
```

It will compile the OS with the debug symbols enabled, all the output logging information will be sent to stdio.

Finally:

```bash
   make gdb
```

Will compile the OS with debug symbol, and launch qemu with remote debugging enabled and will wait connection from gdb to start.


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

![image](https://github.com/dreamos82/Dreamos64/assets/59960116/02ae132c-08c8-47a9-9e4d-7bea0e07865f)


### Acknowledgements

Well so far a big thank you goes to [DeanoBurrito](https://github.com/DeanoBurrito) that implemented several stuff for Dreamos64 (x2apic, fixed some bugs, logging, and more to come...) not to mention his precious help explaining things and reviewing my crappy code :)
And his operating system is worth a mention too: [NorthPort](https://github.com/DeanoBurrito/northport)

Finally if you are interested in how to develop your own kernel, you can head to [https://github.com/dreamportdev/Osdev-Notes/](https://github.com/dreamportdev/Osdev-Notes/) a project  mantained by me, and [DeanoBurrito](https://github.com/DeanoBurrito) on how to write a kernel from scratch.

