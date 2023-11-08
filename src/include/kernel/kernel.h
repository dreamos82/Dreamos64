#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define _HIGHER_HALF_KERNEL_MEM_START   0xffffffff80000000

struct keyboard_status_t {
    uint8_t scancode_set;
    bool    translation_enabled;
};

struct apic_timer_parameters {
    uint32_t timer_ticks_base;
    uint8_t timer_divisor;
};


/**
 * This struct contains the arch paging root table references
 */
struct paging_status_t {
    uint64_t *page_root_address; /**< This is the root table address, as it is provided by the boot code. So it resides in the higher kernel mapped in the higher half */
    uint64_t *hhdm_page_root_address; /**< The hhdm address, that is going to be used with the vmm */
    size_t page_generation; /**< If any changes happens to the page root in the second half, the generation will be increased */
};

typedef struct kernel_status_t {
    struct keyboard_status_t keyboard;
    struct apic_timer_parameters apic_timer;
    struct paging_status_t paging;
    bool use_x2_apic;

    uint64_t kernel_uptime; // Kernel uptime in millisec.
} kernel_status_t;

extern kernel_status_t kernel_settings;

extern uint64_t _kernel_end;
extern uint64_t _kernel_physical_end;

void init_kernel_settings();
uint64_t get_kernel_uptime();
#endif
