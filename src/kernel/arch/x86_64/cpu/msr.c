#include <msr.h>
#include <stdio.h>
#include <stddef.h>

uint64_t rdmsr(uint32_t address){
    uint32_t low=0, high=0;
    printf("TBD\n");
    asm("movl %2, %%ecx;" 
        "rdmsr;"
        : "=a" (low), "=d" (high)
        : "g" (address)
    );

    printf("RDMSR value: 0x%x %x\n", high, low);
    printf("RDMSR value: 0x%x\n", ((high<<32)|low));
    
    return NULL;
}
