// Inspired by https://github.com/stepancheg/no-mutex-c/blob/master/spinlock.c

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

#define UNLOCKED 0
#define LOCKED   1


static bool atomic_compare_and_exchange(int* lock, int compare_value, int exchange_value) {
    // docs: https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
    return __atomic_compare_exchange_n(
        lock,
        &compare_value,
        exchange_value,
        0,                  // disable weak mode; ensure correctness.
        __ATOMIC_SEQ_CST,
        __ATOMIC_SEQ_CST
    );
}

typedef struct spinlock {
    int state;    
} spinlock;


void spinlock_lock(spinlock* spinlock) {
    while (!atomic_compare_and_exchange(&spinlock->state, UNLOCKED, LOCKED));
}

void spinlock_unlock(spinlock* spinlock) {
    if (!atomic_compare_and_exchange(&spinlock->state, LOCKED, UNLOCKED))
        // Somebody is attempting to release the lock but it is not held!
        exit(1);
}

int main(void) {
    spinlock lock = {UNLOCKED};

    spinlock_lock(&lock);
    assert(lock.state == LOCKED);

    spinlock_unlock(&lock);
    assert(lock.state == UNLOCKED);
    return 0;
}