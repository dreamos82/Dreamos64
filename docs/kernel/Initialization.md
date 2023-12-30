#  Kernel Initialization

## Loading the kernel

The kernel is loaded by grub2, using the multiboot2 specifications.

The tags that are enabled at boot are:

* Framebuffer

Although it is optional, in the mid term, support for legacy VGA driver will be dropped (and is not guaranteed to work anymore).

The header is defined in the `src/asm/multiboot_header.s`

Additional tags maybe added in the future (although the long term plan is to replace grub with limine)

We let the bootloader decide for the framebuffer configuration (that is: Width, Height, Depth)

## Booting process (Early boot)

As soon as multiboot pass the control to the kernel loader, the early boot stages are done mostly in the `src/asm/boot.s` file. That will do the following steps:

* Set up paging and map the kernel in the higher half
* Pre parse the multiboot data passed by grub, preparing the data that will be needed by the kernel.
* Enable PAE
* Map the first 4 megabyte of the Framebuffer
* Jump to the kernel in  the higher half

## Booting Process (Late Boot)

Once the setup in the early boot process is done,  the kernel is fully loaded in the higher half, and we can proceed with the rest of the Initialization

The sequence of component that are intialized (refer to `src/main.c`):

* Qemu debug (the main output for now)
* IDT
* Load the PSF font from memory
* Basic System Initialization:
    - Parse the multiboot information received from the bootloader.
    - Parse the mmap and initialize the physical memory manager, marking the pmm areas as busy.
    - Initialize the physical mermoy manager, marking the area in the mmap as already taken.
    - Validate and parse the SDT tables
    - This section needs to be reviewed and check if all the steps are required
* Finish mapping the Framebuffer (there is a potential bug here, need to chek what i do while mapping it)
* Set the Higher Half direct map
* Initialize the kernel VMM
* Initialize the kernel heap
* Initialize the apic
* Initialize the keyboard
* Initialize and load the TSS
* Calibrate the apic timer
* Initialize the VFS layer (although not really used)
* Initialize the scheduler
* Finally start the timer, that it will make the scheulder start working and picking tasks when present in the queue, or run the idle tasks when the queue is empty.

At this point the startup is completed and the kernel starts its infinite loop.
