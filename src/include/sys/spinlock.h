#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#include <stdbool.h>

#define MAX_SPINLOCK_NAME_LEN 32
typedef struct {
    bool    locked;

    char *name;
} spinlock_t;


spinlock_t* spinlock_init(char *name);
void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);
#endif
