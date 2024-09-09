#ifndef SOURCELINE_CACHE_H
#define SOURCELINE_CACHE_H

#include <stdint.h>
#include <sys/types.h>

#include "domain/arena.h"

typedef struct {
  uintptr_t start_addr;
  uintptr_t end_addr;
  // const char *filename;
  int line;
} addr_range_t;

typedef struct {
  arena_t*      arena;
  addr_range_t* ranges;
  size_t        size;
  size_t        capacity;
} addr_range_cache_t;

addr_range_cache_t*
arc_create(arena_t* arena, size_t initial_cap);

void
arc_insert(addr_range_cache_t* cache, addr_range_t range);

addr_range_t*
arc_lookup(addr_range_cache_t* cache, uintptr_t addr);

#endif  // SOURCELINE_CACHE_H