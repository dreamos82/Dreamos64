ENTRY(start)

SECTIONS {
    . = 0x00100000;

    _kernel_start =.;
    .multiboot_header :
    {
        /* Be sure that multiboot header is at the beginning */
        *(.multiboot_header)
    }

    .multiboot.text :
    {
        *(.multiboot.text)
    }

    . += 0xC0000000;
	/* Add a symbol that indicates the start address of the kernel. */
	.text ALIGN (4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.text)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}
	.bss ALIGN (4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
		*(.boot)
	}

    _kernel_end = .;
}
