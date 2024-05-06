# Memory Management

The memory management will be rewritten in the future.

Currently Dreamos64 avail of virtual memory, providing every process with its own address space.

The memory is divided in lower half for _users_ level and the higher half for the _supervisor_ level.

The kernel is loaded at -2G.

Current `PAGE_SIZE` is 2M.


## Initialization workflow

* `mmap_parse` -> Initialize global variable for reading mmap, and print its content.
* `pmm_setup` initialize the phyiscal memory manager layer
    - It first calls `initialize_bitmap`
    - Then

## Physical memory

The physical memory level, is manged using a simple bitmap algorithm. The memory allocated is returned in chunks of PAGE_SIZE.

There are two levels on the phyiscal memory manager:

* the bitmap level that contains the function to set/clear the bits in the bitmap and these functions should be used only by the pmm.
* the pmm level instead contains the function to allocate and free pages of physical memory.

### Memory map

The memory map is based on the one obtained from the multiboot, and during initialization.

## Paging

Paging is provided with fixed size paging (only one size of page is supported at time). The size can be configured between 4k and 2M pages, although lately only 2M pages have been tested.

It avails of `x86_64`paging mechanism.

### Higher Hald Direct Map (HHDM)

An hhdm is provided to the kernel as convenience.

## Virtual Memory  Manager

It sucks, but for now it does its job (partially!)

Currently only the allocation of virtual memory is implemented. There is no `vmm_free` implemented yet.

## KHeap

This is the kernel heap, this is used by the kernel when it needs to allocate resources.


