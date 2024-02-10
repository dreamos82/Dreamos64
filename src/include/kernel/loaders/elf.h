#ifndef __ELF_H__
#define __ELF_H__

#include <stdint.h>
#include <stdbool.h>

#define EI_NIDENT   16
#define ELF_MAGIC_SIZE 4

extern const char _elf_header_mag[];

// The lines below are copied from https://github.com/dreamportdev/Osdev-Notes/blob/master/09_Loading_Elf/01_Elf_Theory.md#elf-overview
typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t Elf64_Sxword;
typedef uint8_t Elf64_UnsignedChar;
// until here

typedef struct {
        unsigned char e_ident[EI_NIDENT];
        Elf64_Half      e_type;
        Elf64_Half      e_machine;
        Elf64_Word      e_version;
        Elf64_Addr      e_entry;
        Elf64_Off       e_phoff;
        Elf64_Off       e_shoff;
        Elf64_Word      e_flags;
        Elf64_Half      e_ehsize;
        Elf64_Half      e_phentsize;
        Elf64_Half      e_phnum;
        Elf64_Half      e_shentsize;
        Elf64_Half      e_shnum;
        Elf64_Half      e_shstrndx;
} Elf64_Ehdr;

void load_elf(uintptr_t elf_start, uint64_t size);

bool parse_section_header(Elf64_Ehdr *elf_start, uint64_t size);

#endif
