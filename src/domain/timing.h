#ifndef TIMING_H
#define TIMING_H

#define get_tickcount() ((long)__builtin_ia32_rdtsc())

#define TICKCOUNT_OF_BLOCK(x)       \
  ({                                \
    long __start = get_tickcount(); \
    x;                              \
    long __end   = get_tickcount(); \
    long __delta = __end - __start; \
    __delta;                        \
  })

#define TIME_A_BLOCK_NS(x)                                  \
  ({                                                        \
    struct timespec __start, __end;                         \
    clock_gettime(CLOCK_MONOTONIC_RAW, &__start);           \
    x;                                                      \
    clock_gettime(CLOCK_MONOTONIC_RAW, &__end);             \
    ulong __delta = (__end.tv_sec - __start.tv_sec) * 1e9 + \
                    (__end.tv_nsec - __start.tv_nsec);      \
    __delta;                                                \
  })

#endif  // TIMING_H