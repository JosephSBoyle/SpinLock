#include <assert.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "lock.h"

#define ACQUISITIONS       100
#define TOTAL_ACQUISITIONS ACQUISITIONS * 2


void test_single_threaded() {
    int lock = UNLOCKED;
    
    spin_lock(&lock);
    assert(lock == LOCKED);

    assert(spin_unlock(&lock) == true);
    assert(lock == UNLOCKED);
    
    // Attempting to unlock an unlocked lock should fail.
    assert(spin_unlock(&lock) == false);
}

// ------ multi-thread test utils ------

typedef struct shared_resource {
    int idx;
    int acquisitions;
} shared_resource;
#define INIT_RESOURCE {0, 0}


/* Used as an argument to `my_thread`. */
typedef struct thread_params {
    int id;
    int* lock;
    shared_resource* r;
} thread_params;


/* `my_thread` is run using `pthread_create` which expects a function signature of:
 * `void* funcname(void* argname)` */
void* my_thread(void* params) {
    thread_params*   p = (thread_params*)params;
    shared_resource* r = p->r;
    
    for (int i=0; i<ACQUISITIONS; i++) {
        spin_lock(p->lock);
        r->acquisitions++;
        spin_unlock(p->lock);
    }
}

// ------ multi-thread test ------

void test_multi_threaded() {
    // Arrange: create 2 threads
    // Act    : start both threads and wait for them to complete
    // Assert : check that the shared resource was acquired the correct number of times
 
    int lock = UNLOCKED;
    
    pthread_t thread1;
    pthread_t thread2;

    shared_resource r = INIT_RESOURCE;
    thread_params  p1 = {1, &lock, &r};
    thread_params  p2 = {2, &lock, &r};

    pthread_create(&thread1, NULL, &my_thread, &p1);
    pthread_create(&thread2, NULL, &my_thread, &p2);

    pthread_join(thread1, 0);
    pthread_join(thread2, 0);

    assert(lock == UNLOCKED);
    assert(r.acquisitions == TOTAL_ACQUISITIONS);
}


int main(void) {
    test_single_threaded();
    test_multi_threaded();

    printf("=====================All tests passed!=====================\n");
    return 0;
}