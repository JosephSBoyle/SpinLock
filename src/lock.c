// Inspired by https://github.com/stepancheg/no-mutex-c/blob/master/spinlock.c

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// possible lock states.
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

/* Obtain the lock. */
void spin_lock(int* lock) {
    while (!atomic_compare_and_exchange(lock, UNLOCKED, LOCKED));
}


/* Release the lock, returning whether or not the operation succeeded.
   Returns false if the lock was already unlocked. */
bool spin_unlock(int* lock) {
    return atomic_compare_and_exchange(lock, LOCKED, UNLOCKED);
}

