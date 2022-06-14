#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#include <stdbool.h>

typedef struct {
    bool    locked;
} spinlock_t;


spinlock_t* spinlock_init(char *name);
void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);
void spinlock_free(spinlock_t *lock);
#endif
