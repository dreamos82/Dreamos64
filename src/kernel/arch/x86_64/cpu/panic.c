#include <panic.h>
#include <stdio.h>

void panic(char *message) {
    printf("[PANIC] %s\n", message);
    while(1);
}
