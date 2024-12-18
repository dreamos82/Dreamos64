ENTRY(loop)

SECTIONS {

	. = 4M;

	.text : ALIGN (4K)
	{
		*(.text)
		*(.text.*)
	}

	.rodata : ALIGN (4K)
	{
		*(.rodata)
		*(.rodata.*)
	}

	.data : ALIGN (4K)
	{
		*(.data)
		*(.data.*)
	}

	.bss : ALIGN (4K)
	{
		*(.bss)
	}

}

