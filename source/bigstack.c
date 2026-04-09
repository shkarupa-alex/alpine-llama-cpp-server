#define _GNU_SOURCE
#include <pthread.h>
#include <dlfcn.h>
#include <stddef.h>

/*
 * Override pthread_create to use 8MB thread stacks instead of musl libc's
 * default 128KB. To be loaded via LD_PRELOAD at runtime.
 */
int pthread_create(pthread_t *restrict t, const pthread_attr_t *restrict a,
                   void *(*start)(void*), void *restrict arg) {
    static int (*real)(pthread_t*, const pthread_attr_t*, void*(*)(void*), void*);
    if (!real) real = dlsym(RTLD_NEXT, "pthread_create");
    if (a) return real(t, a, start, arg);
    pthread_attr_t na;
    pthread_attr_init(&na);
    pthread_attr_setstacksize(&na, 8388608);
    int r = real(t, &na, start, arg);
    pthread_attr_destroy(&na);
    return r;
}
