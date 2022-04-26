#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <stdint.h>
#include <stdbool.h>

struct keyboard_status_t {
    uint8_t scancode_set;
    bool    translation_enabled;
};

struct apic_timer_parameters {
    uint32_t timer_ticks_base;
    uint8_t timer_divisor;
};

typedef struct kernel_status_t {
    struct keyboard_status_t keyboard;
    struct apic_timer_parameters apic_timer;
    bool use_x2_apic;
} kernel_status_t;

extern kernel_status_t kernel_settings;

extern unsigned int _kernel_end;
extern unsigned int _kernel_physical_end;

#endif
