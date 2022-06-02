#include <kernel.h>

kernel_status_t kernel_settings;

void init_kernel_settings() {
    kernel_settings.kernel_uptime = 0l;
}

uint64_t get_kernel_uptime() {
    return kernel_settings.kernel_uptime;
}
