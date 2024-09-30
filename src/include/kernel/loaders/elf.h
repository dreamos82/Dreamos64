#ifndef __ELF_H__
#define __ELF_H__

#define X86_64

#include <stdint.h>
#include <stdbool.h>

#define EI_NIDENT   16

#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_OSABI 7
#define EI_ABI 8
#define EI_PAD 9

#define ELF_MAGIC_SIZE 4

#define ELFCLASS64   2

#define ELFDATA2LSB     1

#define EV_CURRENT      1

#define PT_LOAD 1

#ifdef X86_64
#define ELF_MACHINE     62
#endif


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

typedef enum {
        ELF
} executable_loader_type;

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

typedef struct {
        Elf64_Word      p_type;
        Elf64_Word      p_flags;
        Elf64_Off       p_offset;
        Elf64_Addr      p_vaddr;
        Elf64_Addr      p_paddr;
        Elf64_Xword     p_filesz;
        Elf64_Xword     p_memsz;
        Elf64_Xword     p_align;
} Elf64_Phdr;

#define PT_LOAD 1

#define FF_X    1
#define FF_R    2
#define FF_W    4

void load_elf(uintptr_t elf_start, uint64_t size);

// This function maybe will  change, and it will be a wrapper for supporting different executable formats. This is the reasaon of the type parameter
bool parse_section_header(Elf64_Ehdr *elf_start, uint64_t size, executable_loader_type type);

Elf64_Half loop_phdrs(Elf64_Ehdr* e_phdr, Elf64_Half phdr_entries);
Elf64_Phdr *read_phdr(Elf64_Ehdr* e_hdr, Elf64_Half phdr_entry_number);
#endif
