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

This function, is very simple and it's main purpose is to initialize the global variables that will contain the pointer to the `memory map` and the total number of entries in it.

The memory map is used to understand what parts of the physical memory are reserved and what are available to the kernel.


#### Chicken Egg Problem: the HHDM and the PMM

Before explaining what `pmm_setup` does, let's see first an issue that arised and how we decided to fix it.

So one of the problems, especially if we use 4k pages (it was not present using 2M pages) is that we need to map a lot of memory to cater for all the structures needed by the PMM, but also by the VMM, and everything else. When the kernel is loaded we usually have some space already mapped just after its end, but it is pure coincidence, and we can eventually reach an unmapped part of memory, causing a `#PF`, the kernel needs to map that new region in advance, but that can be a problem on early stages, let' s see why.

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

And again we face the problem that the _HHDM_ needs to be initialized, and to do it we still need the PMM, but if we could be able to initialize the `hhdm`  before everyhing else it will provide us two important new information:

* The first is that once the hhdm is initialized we are sure that for the rest of initialization of any memory management part, we don't need to map anything else, because we can already access all the memory we want.
* The `hhdm` will never change once initailized, yeah the content of the memory can change, but the mapping will always be the same. So there will never be the need to free any address within the hhdm.

Another important thing that we already know is that usually any memory right after the kernel is free, and if it is not is because something is loaded there, we should be able to tell it by parsing some data structures provided by grub (the memory map, and the modules). At this point we can safely assume that unless the addresses above the end of the kernel are reserved in the mmap, or used by some module, that area can be used.

And here is the solution, until the memory manager is not initialized, we will keep returning addresses that are just after the kernel for creating the page tables required for the `hhdm` Initialization (again with the exclusion of the parts that we already knows are marked as used by the bios/bootloader).

Once the _hhdm_  is initialized, we proceed with the PMM Initialization, but now reassured that we will not encounter any unexpected memory mapping, because we will use the memory map just initialized.

#### pmm_setup()

The first thing that the `pmm_setup()` function does is setting the start address of the `anon memory`  used for allocations of the page tables until the pmm is fully initialized and functional.

These addresses are tracked in `anon_memory_loc` (for the virtual address to be returned) and `anon_physical_memory_loc` (for the physical counterpart).

Then it calls the `hhdm_map_physical_memory()` function, thata prepare the memory map in the higher half. After this point we are able to access the whole physical memory using it's `hhdm` represenation. To clarify, a phyiscal address hhdm representation is given by a simple formula:

```c
hhdm_var_addres = var_address_phys + HHDM_OFFSET
```

Where `HHDM_OFFSET` is the starting addresss of the map.

At this point is possible to initalize in the following order:

* the physical memory bitmap: that keeps track of the physical memory that is allocated and free
* then mark as reserved in the bitmap the memory used for the bitmap itself

Before returning this function set the status of the `pmm_initialized` variable as `true`, meaning that from now on, any page table allocation request from will pass through the `pmm`, instead of using the `anonymous allocation`.

This is also the end of the physical memory initialization.

### Physical Memory Management

The physical memory level, is manged using a simple bitmap algorithm. The memory allocated is returned in chunks of `PAGE_SIZE`.

There are two levels on the physical memory manager:

* the bitmap level that contains the function to set/clear the bits in the bitmap and these functions should be used only by the pmm.
* the pmm level instead contains the function to allocate and free pages of physical memory.

### The bitmap

This is the lowest level of memory management. It is simply a _bit map_, an array where every bit on each item is representing a memory page. 

This level just keep tracks of used and free pages. It doesn't really allocate. 

If the bitmap entry is 1 the page is used, otherwise is free. 

### Memory map

The memory map is based on the one obtained from the multiboot, and during initialization.

## Paging

Paging is provided with fixed size paging (only one size of page is supported at time). The size can be configured between 4k and 2M pages, although lately only 2M pages have been tested.

It avails of `x86_64`paging mechanism.

### Higher Half Direct Map (HHDM)

An _hhdm_ is provided to the kernel as convenience.

It starts at `higherHalfDirectMapBase`.

The variable is defined in `main.c`, and is initialized as: 

```c
uintptr_t higherHalfDirectMapBase = ((uint64_t) HIGHER_HALF_ADDRESS_OFFSET + VM_KERNEL_MEMORY_PADDING);
```

## Virtual Memory  Manager

It sucks, but for now it does its job (partially!)

Currently only the allocation of virtual memory is implemented. There is no `vmm_free` implemented yet.

### Overview

The basic idea is that the memory is split in allocable `regions`. Every region is defined inside a _container_ (`VmmContainer`). Allocation is done in `PAGE_SIZE` chunks. 

All regions are stored in a linked list. Every Container is exactly 1 page in size. 

Inside the regions there is an array of `VmmItem` (that contains info about the used memory), and a pointer to the next container. 

The `VmmItem`, has 3 basic information: 

* its base pointer (where the virtual address start)
* its size (in bytes, but the allocation is always page aligned)
* its flags

It is defined as follows: 

```c
typedef struct VmmItem{
    uintptr_t base;
    size_t size;
    size_t flags;
} VmmItem;
```

The `VmmContainer` structure is as follows: 

```c
typedef struct VmmContainer {
    VmmItem vmm_root[(PAGE_SIZE_IN_BYTES/sizeof(VmmItem) - 1)];
    struct VmmContainer *next;
} __attribute__((__packed__)) VmmContainer;
```

The snapshot of the status of the _VMM_ is stored inside the `VmmStatus` struct, defined as below: 

```c
    struct VmmStatus {
        size_t vmm_items_per_page; /**< Number of page items contained in one page */
        size_t vmm_cur_index; /**< Current position inside the array */

        size_t next_available_address; /**< The next available address */

        uint64_t end_of_vmm_data; /**< We should never reach here, where the vmm_data finish */

        VmmContainer *vmm_container_root; /**< Root node of the vmmContainer */
        VmmContainer *vmm_cur_container; /**< Current pointer */
    } status;
```

The main thing about the above structure is that it contains the root node of the `VmmContainer` list, and then the pointer to the node being used currently. In addition it keeps track of the last index used and also the pointer to the `next_available_address`. 

The struct is defined inside the `VmmContainer` object.

### Initialization

A portion of the address space is reserved for Virtual Memory structs, its size is defined in `vmm.h`: 

```c
#define VMM_RESERVED_SPACE_SIZE 0x14200000000
```

And the address space sits on top of the hhdm: 

```c
vmm_info->vmmDataStart = align_value_to_page(higherHalfDirectMapBase + memory_size_in_bytes + VM_KERNEL_MEMORY_PADDING);
```

There is one kernel VMM, and then every process will have its own.

A `VmmItem` is free if its base and size are both 0. 

## KHeap

This is the kernel heap, this is used by the kernel when it needs to allocate resources.

## Memory Organization

Below MMIO_HIGHER_HALF_ADDRESS_OFFSET the address space is reserved by user space memory.

* `MMIO_HIGHER_HALF_ADDRESS_OFFSET = 0xFFFF800000000000`
    - This address mark the start of the address space reserved for allocating MMIO devices
    - The MMIO address space size is: `0x280000000` (defined in `MMIO_RESERVED_SPACE_SIZE`)
* `HIGHER_HALF_ADDRESS_OFFSET = (MMIO_HIGHER_HALF_ADDRESS_OFFSET + MMIO_RESERVED_SPACE_SIZE) = 0xFFFF800280000000`
    - This addres is the start of the address space that will be used by the kernel while in supervisor mode.
* `KERNEL_VIRTUAL_ADDR =  0xFFFFFFFF80000000`
    - From that address, we hae the original mapping of the kernel, in the higher half of memory.

