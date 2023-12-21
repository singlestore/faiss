#pragma once

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

// https://openmp.org/wp-content/uploads/OpenMP4.0.0.pdf
//

// C.1 Example of the omp.h Header File
//

typedef void* omp_lock_t;

// A.1 C/C++ Stub Routines
//

inline void omp_set_num_threads(int num_threads)
{
    assert(false);
}

inline int omp_get_num_threads(void)
{
    return 1;
}

inline int omp_get_max_threads(void)
{
    return 1;
}

inline int omp_get_thread_num(void)
{
    return 0;
}

inline int omp_in_parallel(void)
{
    assert(false);
    return 0;
}

inline void omp_set_nested(int nested)
{
    assert(false);
}

inline int omp_get_nested(void)
{
    assert(false);
    return 0;
}

struct __omp_lock
{
    int lock;
};

enum
{
    UNLOCKED = -1,
    INIT,
    LOCKED
};

inline void omp_init_lock(omp_lock_t* arg)
{
    struct __omp_lock* lock = (struct __omp_lock*)arg;
    lock->lock = UNLOCKED;
}

inline void omp_destroy_lock(omp_lock_t* arg)
{
    struct __omp_lock* lock = (struct __omp_lock*)arg;
    lock->lock = INIT;
}

inline void omp_set_lock(omp_lock_t* arg)
{
    struct __omp_lock* lock = (struct __omp_lock*)arg;
    if (lock->lock == UNLOCKED)
    {
        lock->lock = LOCKED;
    }
    else if (lock->lock == LOCKED)
    {
        fprintf(stderr, "error: deadlock in using lock variable\n");
        exit(1);
    }
    else
    {
        fprintf(stderr, "error: lock not initialized\n");
        exit(1);
    }
}

inline void omp_unset_lock(omp_lock_t* arg)
{
    struct __omp_lock* lock = (struct __omp_lock*)arg;
    if (lock->lock == LOCKED)
    {
        lock->lock = UNLOCKED;
    }
    else if (lock->lock == UNLOCKED)
    {
        fprintf(stderr, "error: lock not set\n");
        exit(1);
    }
    else
    {
        fprintf(stderr, "error: lock not initialized\n");
        exit(1);
    }
}
