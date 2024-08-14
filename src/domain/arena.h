#ifndef ARENA_H
#define ARENA_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct {
  uintptr_t *base;
  size_t     offset;
  size_t     capacity;
} arena_t;

#define ARENA_PAGE_SIZE ((size_t)sysconf(_SC_PAGESIZE))

static inline size_t
align_to_page_size(size_t size) {
  return (size + ARENA_PAGE_SIZE - 1) & ~(ARENA_PAGE_SIZE - 1);
}

arena_t
arena_create(size_t capacity) {
  arena_t arena;
  size_t  aligned_capacity = align_to_page_size(capacity);

  arena.base = mmap(NULL, aligned_capacity, PROT_READ | PROT_WRITE,
                    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  assert(arena.base != MAP_FAILED && "MMAP failed");

  arena.offset   = 0;
  arena.capacity = aligned_capacity;

  return arena;
}

void *
arena_alloc(arena_t *arena, size_t size) {
  size_t aligned_size =
      (size + sizeof(uintptr_t) - 1) & ~(sizeof(uintptr_t) - 1);

  size_t units = aligned_size / sizeof(uintptr_t);

  assert(arena->offset + units <= arena->capacity && "Out of memory in arena!");

  void *ptr = (void *)(arena->base + arena->offset);
  arena->offset += units;

  return ptr;
}

arena_t
arena_scratch(arena_t *arena, size_t size) {
  size_t  temp_capacity = align_to_page_size(size);
  arena_t temp_arena    = {.base     = arena->base + arena->offset,
                           .offset   = 0,
                           .capacity = temp_capacity};

  assert(arena->offset + temp_capacity <= arena->capacity &&
         "Out of memory in main arena!");
  arena->offset += temp_capacity;

  return temp_arena;
}

void
arena_reset(arena_t *arena) {
  arena->offset = 0;
}

void
arena_destroy(arena_t *arena) {
  munmap(arena->base, arena->capacity);
}

#endif  // ARENA_H