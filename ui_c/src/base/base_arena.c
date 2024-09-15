#include "base_arena.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

// Helpers
#define ARENA_PAGE_SIZE ((U64)sysconf(_SC_PAGESIZE))

static inline U64
align_to_page_size(U64 size) {
  return (size + ARENA_PAGE_SIZE - 1) & ~(ARENA_PAGE_SIZE - 1);
}

// static inline Arena
// arena_scratch(Arena *arena, size_t size) {
//   size_t temp_capacity = align_to_page_size(size);
//   Arena  temp_arena    = {.base     = arena->base + arena->offset,
//                           .offset   = 0,
//                           .capacity = temp_capacity};

//   assert(arena->offset + temp_capacity <= arena->capacity &&
//          "Out of memory in main arena!");
//   arena->offset += temp_capacity;

//   return temp_arena;
// }

// static inline void
// arena_reset(Arena *arena) {
//   arena->offset = 0;
// }

// static inline void
// arena_destroy(Arena *arena) {
//   munmap(arena->base, arena->capacity);
// }

// Arena creation/destruction

static inline Arena *
arena_alloc(U64 capacity) {
  U64 aligned_capacity = align_to_page_size(capacity);

  void *base = mmap(NULL, aligned_capacity, PROT_READ | PROT_WRITE,
                    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  assert(base != MAP_FAILED && "MMAP failed");

  Arena *arena    = (Arena *)base;
  arena->offset   = 0;
  arena->capacity = aligned_capacity;

  return arena;
}

static inline void
arena_release(Arena *arena) {
  munmap(arena->base, arena->capacity);
  // If a user tries to use the arena again it will throw on assets.
  arena->capacity = 0;
  arena->offset   = 0;
}

// Arena core functions
static inline void *
arena_push(Arena *arena, U64 size) {
  size_t aligned_size =
      (size + sizeof(uintptr_t) - 1) & ~(sizeof(uintptr_t) - 1);

  assert(arena->offset + aligned_size <= arena->capacity &&
         "Out of memory in arena!");

  void *ptr = (void *)(arena->base + arena->offset);
  arena->offset += aligned_size;

  return ptr;
}

static inline U64
arena_pos(Arena *arena) {
  return arena->offset;
}

// Arena offset functions
static inline void
arena_reset(Arena *arena) {
  arena_pop_to(arena, 0);
}

static inline void
arena_pop(Arena *arena, U64 amount) {
  U64 old_offset = arena_pos(arena);
  U64 new_offset = old_offset;
  if (amount < old_offset) {
    new_offset = old_offset - amount;
  }
  arena_pop_to(arena, new_offset);
}

static inline void
arena_pop_to(Arena *arena, U64 pos) {
  assert(arena->offset >= pos);
  arena->offset = pos;
}

// Scratch arenas (temporary arenas)
static inline Scratch
arena_scratch_begin(Arena *arena) {
  U64 pos = arena_pos(arena);
  return (Scratch){arena, pos};
}

static inline void
arena_scratch_end(Scratch scratch_arena) {
  arena_pop_to(scratch_arena.arena, scratch_arena.offset);
}

// Macro helpers
#define push_array(arena, T, n) arena_push(arena, (sizeof(T) * n))