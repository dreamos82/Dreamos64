#include <elf.h>
#include <logging.h>
#include <utils.h>

const char _elf_header_mag[ELF_MAGIC_SIZE]={0x7f, 'E', 'L', 'F'};

void load_elf(uintptr_t elf_start, uint64_t size) {
    Elf64_Ehdr *elf_header = (Elf64_Ehdr *) elf_start;
    parse_section_header(elf_header, size);
}

bool parse_section_header(Elf64_Ehdr *elf_start, uint64_t size) {
    char *_elf_nident_header = (char *) elf_start;
    for (int i = 0; i < ELF_MAGIC_SIZE; i++) {
        if (elf_start->e_ident[i] == _elf_header_mag[i]) {
            pretty_logf(Verbose, " i: %d: elf_start = %c : _elf_header_mag = %c", i, elf_start->e_ident[i], _elf_header_mag[i]);
        }
    }
    pretty_log(Verbose, "Nident flags:" );
    pretty_logf(Verbose, "\tCLASS: 0x%x", elf_start->e_ident[4]);
    pretty_logf(Verbose, "\tDATA: 0x%x", elf_start->e_ident[5]);
    pretty_logf(Verbose, "\tVERSION: 0x%x", elf_start->e_ident[6]);
    pretty_logf(Verbose, "\tOS ABI: 0x%x", elf_start->e_ident[7]);
    pretty_logf(Verbose, "\tABI: 0x%x", elf_start->e_ident[8]);
    return false;
}
