ENTRY(start)

SECTIONS {
    . = 1M;

    _kernel_start = .;
    _kern_virtual_offset = 0xffffffff80000000;
    .multiboot_header :
    {
        /* Be sure that multiboot header is at the beginning */
        *(.multiboot_header)
    }

    .multiboot.text :
    {
        *(.multiboot.text)
    }

    . += _kern_virtual_offset;
	/* Add a symbol that indicates the start address of the kernel. */
	.text ALIGN (4K) : AT (ADDR (.text) - _kern_virtual_offset)
	{
		*(.text)
		*(.text.*)
		*(.ltext)
		*(.ltext.*)
	}
	.rodata ALIGN (4K) : AT (ADDR (.rodata) - _kern_virtual_offset)
	{
		*(.rodata)
		*(.rodata.*)
		*(.lrodata)
		*(.lrodata.*)
	}
	.data ALIGN (4K) : AT (ADDR (.data) - _kern_virtual_offset)
	{
		*(.data)
		*(.data.*)
		*(.ldata)
		*(.ldata.*)
	}
	.bss ALIGN (4K) : AT (ADDR (.bss) - _kern_virtual_offset)
	{
		*(.bss)
		*(.lbss)
		*(.bss.*)
		*(.lbss.*)
	}

    _kernel_end = .;
    _kernel_physical_end = . - _kern_virtual_offset;
}
