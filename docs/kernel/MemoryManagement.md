# Memory Management

The memory management will be rewritten in the future.

Currently Dreamos64 avail of virtual memory, providing every process with its own address space.

The memory is divided in lower half for _users_ level and the higher half for the _supervisor_ level.

The kernel is loaded at -2G.

Current `PAGE_SIZE` is 2M.


## Initialization workflow

The very first step before initializing the memory management, is to get the `tag_meminfo` and `tag_mmap`  from the multiboot structure, and make them available to the kernel (saving them as global variables).

The next step is to compute the available memory using information from the `tag_meminfo`  struct (this part needs to be fixed).

During the kernel initialization, the functions that are called that initialize the various layers of memory are:

* `mmap_parse`: Initialize global variable for reading mmap, and print its content.
* `pmm_setup`: initialize the phyiscal memory manager layer and the hhdm.
* `vmm_init`: is called to initialize virtual memory.
* `init_kheap`:  is called to initialize the kernel heap.

What they do will be detailed in the next sections.

### The PMM

#### mmap_parse()

The first function, is very simple and it' s main purpose is to initialize the global variables that will contain the pointer to the `memory map` and the total number of entries in it.

The memory map is used to understand what parts of the physical memory are reserved and what are available to the kernel.


#### Chicken Egg Problem: the HHDM and the PMM

Before explaining what `pmm_setup`, does , let's see first an issue that arised and how we decided to fix it.

So one of the problems, especially if we use 4k pages (it was not present using 2M pages) is that we need to map a lot of memory to cater for all the structures needed by the PMM, but also by the VMM, and everything else. When the kernel is loaded we usually have some space already mapped just after it's end, but it is pure coincidence, and if we eventually reach an unmapped part of memory, without considering the potential `#PF`, it requires the kernel to map that new region but that can be a problem on early stages, let' s see why.

On many architectures paging is usually obtained using a `multi-level`  approach, where a portion of a address is just an entry into a specific table, and the value is the address of the next level table, until we reach the last part of the address that is the offset within the address contained on the last-level entry table (we don' t cover the details here, if interested in learning more this is explained in the [Paging](https://github.com/dreamportdev/Osdev-Notes/blob/master/04_Memory_Management/03_Paging.md) chapter of the _Osdev Notes_).

This means that there are two possible scenarios when mapping a new memory range:

* All the various tables at all the level are already allocated and present, so we just need to map the phyical address provided to the mapping function to the last level table and set it as present. And this case is fine.
* One or more tables at some levels are not present yet, so before mapping the physical address  we need to prepare a new table for each table that was not present yet. And to prepare a new table means that we need a physical area for it to reside usually at least of 4k size, and initialize it at 0. And here we have a problem.

The problem is that to prepare a new page table, we need an address, that is different from the one we want to map, is another one to contain a new table. How do we get it? To get it we should ask the PMM a new address, and this is fine unless is the PMM itself that while initializing it's structure is in need of some memory area mapped.

We are then in the scenario that to initalize the PMM we can risk of needing the PMM to allocate some memory for itself indirectly. And of course this can't work.

A potential solution is to use the HHDM (a map of the whole phsical memory in the higher half that start at a specific offset), so in this way if we want to access any phyiscal address it is going to be very easy:

```c
char *pointer_to_variable = (char *) hhdm_get_variable(physical_address);
```

And we will get access to the content of the `physical_address` variable using it's *higher half representative*

And again we face the problem that the _HHDM_ needs to be initialized, and to do it we still need the PMM, but if we could be able to initialize the `hhdm`  before everyhing else in the PMM it will provide us two important new information:

* The first is that once the hhdm is initialized we are sure that for the rest of initialization of any memory management part, we don't neet to map anything else, because we can access all the memory we want already.
* The `hhdm` will never change once initailized, yeah the content of the memory can changes, but the mapping will always be the same. So there will nevery be the need to free any address within the hhdm.

Another important thing that we already know is that usually any memory right after the kernel is usually free, and if it is not is because something is loaded there, and we should be able to tell it by parsing some data structures provided by grub (the memory map, and the modules). At this point we can safely assume that unless the addresses above the end of the kernel are reserved in the mmap, or used by some module (and that can be easily achieved by parsing the tables already present in memory), that area can be used.

And here is the solution, until the memory maanger is not initialized, we will keep returning addresses that are just after the kernel for creating the page tables required for the `hhdm` Initialization (again with the exclusion of the parts that we already knows are marked as used by the bios/bootloader).

Once the _hhdm_  is initialized, we proceed with the PMM Initialization, but now reassured that we will not encounter any unexpected memory mapping, because we will use the memory map just initialized.

### pmm_setup()

The first thing that the `pmm_setup()` function is setting the start address of the `anon memory`  used for allocations of the page tables until the pmm is fully initialized and functional.

These addresses are tracked in `anon_memory_loc` (for the virtual address to be returned) and `anon_physical_memory_loc` (for the physical counterpart).

Then it calls the `hhdm_map_physical_memory()` function, thata prepare the mememory map inthe higher half. After this point we are able to access the whole physical memory using it's `hhdm` represenation. To clarify, a phyiscal address hhdm representation is given by a simple formula:

```c
hhdm_var_addres = var_address_phys + HHDM_OFFSET
```

Where `HHDM_OFFSET` is the starting addresss of the map.

At this point is possible to initalize in the following order:

* the physical memory bitmap: that keeps track of the physical memory that is allocated and free
* then mark as reserved in the bitmap the memory used for the bitmap itself


### Physical Memory Management

The physical memory level, is manged using a simple bitmap algorithm. The memory allocated is returned in chunks of `PAGE_SIZE`.

There are two levels on the physical memory manager:

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


