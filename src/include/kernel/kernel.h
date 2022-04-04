#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <stdint.h>
#include <stdbool.h>

struct keyboard_status_t {
    uint8_t scancode_set;
    bool    translation_enabled;
};

typedef struct kernel_status_t {

    struct keyboard_status_t keyboard;
} kernel_status_t;

extern kernel_status_t kernel_settings;
#endif
