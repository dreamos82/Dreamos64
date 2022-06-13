#include <spinlock.h>
#include <kheap.h>

spinlock_t* spinlock_init(char *name) {
    spinlock_t* new_spinlock = kmalloc(sizeof(spinlock_t));
    new_spinlock->locked = false;
    new_spinlock->name = name;
    return new_spinlock;
}

void spinlock_acquire(spinlock_t *lock) {
    while (__atomic_test_and_set(&lock->locked, __ATOMIC_ACQUIRE));
}

void spinlock_release(spinlock_t *lock) {
    __atomic_clear(&lock->locked, __ATOMIC_RELEASE);
}

void spinlock_free(spinlock_t* spinlock) {
    kfree(spinlock);
}
