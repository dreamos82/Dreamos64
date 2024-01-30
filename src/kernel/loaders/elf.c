#include <elf.h>
#include <logging.h>
#include <utils.h>

const char _elf_header_mag[4]={0x7f, 'E', 'L', 'F'};

void load_elf(uintptr_t elf_start, uint64_t size) {
    char *_elf_nident_header = (char *) elf_start;
    Elf64_Ehdr *elf_header = (Elf64_Ehdr *) elf_start;
    for (int i = 0; i < 4; i++) {
        if (elf_header->e_ident[i] == _elf_header_mag[i]) {
            pretty_logf(Verbose, " i: %d: elf_start = %c : _elf_header_mag = %c", i, elf_header->e_ident[i], _elf_header_mag[i]);
        }
    }

}
