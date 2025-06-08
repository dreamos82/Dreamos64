#ifndef __TSS_H__
#define __TSS_H__

#include <stdint.h>
#include <stddef.h>

#define TSS_ENTRY_LOW 5
#define TSS_ENTRY_HIGH 6

/** This structure is copied from OSDev Notes, Part 6: Userspace.
   * https://github.com/dreamos82/Osdev-Notes/blob/master/06_Userspace/03_Handling_Interrupts.md
   */
typedef struct tss
{
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t io_bitmap_offset;
}__attribute__((__packed__)) tss_t;

//typedef struct tss tss_t;

extern tss_t kernel_tss;

extern void _load_task_register();

void initialize_tss();
void load_tss();

#endif
