#include <test_common.h>
#include <stdio.h>
#include <bitmap.h>

void _printStringAndNumber(char *string, unsigned long number){
    printf("%s0x%X\n", string, number);
}

void _printStr(const char *string){
    printf("%s", string);
}

void _printNewLine(){
    printf("\n");
}

uint32_t _compute_kernel_entries(uint64_t end_of_kernel_area){
    uint32_t kernel_entries = 0x1190AC / PAGE_SIZE_IN_BYTES;
    uint32_t kernel_mod_entries = 0x1190AC % PAGE_SIZE_IN_BYTES;
    printf("kernel_entries: 0x%X\n", kernel_entries);
    printf("kernel_mod_entries: 0x%X\n", kernel_mod_entries);
    if (  kernel_mod_entries != 0){
        return kernel_entries + 2;
    } 
    return kernel_entries + 1;
   
}

void *map_vaddress(void *address, unsigned int flags){
    return NULL;
}


void map_vaddress_range(void *virtual_address, unsigned int flags, size_t required_pages) {
    return;
}

void *map_phys_to_virt_addr(void* physical_address, void* address, size_t flags) {
    return NULL;
}
void init_log(size_t defaultOutputs, log_level_t trimBelowLevel, bool useVgaVideo) {
    return;
}
void set_log_trim_level(size_t newTrim) {
    return;
}
void logline(log_level_t level, const char* msg) {
    return;
}

void loglinef(log_level_t level, const char* msg, ...){
    return;
}

void spinlock_acquire(spinlock_t *lock) {
    return;
}

void spinlock_release(spinlock_t *lock) {
    return;
}

void spinlock_free(spinlock_t* spinlock) {
    return;
}
