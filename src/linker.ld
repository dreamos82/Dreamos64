ENTRY(start)

SECTIONS {
    . = 1M;

    .boot :
    {
        /* Be sure that multiboot header is at the beginning */
        *(.multiboot_header)
    }

    .text :
    {
        *(.text)
    }

    end = .;
}
