// #include "base.h"

// #ifndef BASE_ARENA_H
// #define BASE_ARENA_H

// typedef struct {
//   U64 *base;
//   U64  offset;
//   U64  capacity;
// } Arena;

// typedef struct {
//   Arena *arena;
//   U64    offset;
// } Scratch;

// // Arena creation/destruction

// static inline Arena
// arena_alloc(U64 capacity);

// static inline void
// arena_release(Arena *arena);

// // Arena core functions
// static inline void *
// arena_push(Arena *arena, U64 size);

// static inline U64
// arena_pos(Arena *arena);

// // Arena offset functions
// static inline void
// arena_reset(Arena *arena);

// static inline void
// arena_pop(Arena *arena, U64 amount);

// static inline void
// arena_pop_to(Arena *arena, U64 pos);

// // Scratch arenas (temporary arenas)
// static inline Scratch
// arena_scratch_begin(Arena *arena);

// static inline void
// arena_scratch_end(Scratch scratch_arena);

// #endif  // BASE_ARENA_H


