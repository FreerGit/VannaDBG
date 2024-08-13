#ifndef ARENA_H
#define ARENA_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct {
  uintptr_t *base;
  size_t     offset;
  size_t     capacity;
} Arena_t;

#define ARENA_PAGE_SIZE ((size_t)sysconf(_SC_PAGESIZE))

// Function to align the size to the nearest multiple of the page size.
static inline size_t
align_to_page_size(size_t size) {
  return (size + ARENA_PAGE_SIZE - 1) & ~(ARENA_PAGE_SIZE - 1);
}

// Function to create a new arena with the specified capacity.
Arena_t
arena_create(size_t capacity) {
  Arena_t arena;
  size_t  aligned_capacity = align_to_page_size(capacity);

  arena.base = mmap(NULL, aligned_capacity, PROT_READ | PROT_WRITE,
                    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  assert(arena.base != MAP_FAILED);

  arena.offset   = 0;
  arena.capacity = aligned_capacity;

  return arena;
}

// Function to allocate memory from the arena.
void *
arena_alloc(Arena_t *arena, size_t size) {
  size_t aligned_size = (size + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);
  ;
  assert(arena->offset + aligned_size <= arena->capacity &&
         "Out of memory in arena!");

  void *ptr = (void *)(arena->base + arena->offset);
  arena->offset += aligned_size;

  return ptr;
}

// Function to create a temporary (scratch) arena.
Arena_t
arena_scratch(Arena_t *arena, size_t size) {
  size_t  temp_capacity = align_to_page_size(size);
  Arena_t temp_arena    = {.base     = arena->base + arena->offset,
                           .offset   = 0,
                           .capacity = temp_capacity};

  assert(arena->offset + temp_capacity <= arena->capacity &&
         "Out of memory in main arena!");
  arena->offset += temp_capacity;

  return temp_arena;
}

// Function to reset the arena, making all memory available again.
void
arena_reset(Arena_t *arena) {
  arena->offset = 0;
}

// Function to destroy the arena and free the virtual memory.
void
arena_destroy(Arena_t *arena) {
  munmap(arena->base, arena->capacity);
}

#endif  // ARENA_H