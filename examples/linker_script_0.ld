ENTRY(main)

SECTIONS {

	. = 4M;

	.text : ALIGN (2M)
	{
		*(.text)
		*(.text.*)
	}

	.rodata : ALIGN (2M)
	{
		*(.rodata)
		*(.rodata.*)
	}

	.data : ALIGN (2M)
	{
		*(.data)
		*(.data.*)
	}

	.bss : ALIGN (2M)
	{
		*(.bss)
	}

}

