#include <addr_range_cache.h>

addr_range_cache_t*
arc_create(arena_t* arena, size_t initial_cap) {
  addr_range_cache_t* cache = arena_alloc(arena, sizeof(addr_range_cache_t));
  cache->ranges   = arena_alloc(arena, sizeof(addr_range_t) * initial_cap);
  cache->capacity = initial_cap;
  cache->size     = 0;
  cache->arena    = arena;
  return cache;
}

void
arc_insert(addr_range_cache_t* cache, addr_range_t range) {
  if (cache->size == cache->capacity) {
    size_t        new_capacity = cache->capacity * 2;
    addr_range_t* new_ranges =
        arena_alloc(cache->arena, sizeof(addr_range_t) * new_capacity);
    memcpy(new_ranges, cache->ranges, sizeof(addr_range_t) * cache->size);
    cache->ranges   = new_ranges;
    cache->capacity = new_capacity;
  }

  cache->ranges[cache->size] = range;
  cache->size++;

  // sort to keep ranges sorted by start_addr
  for (size_t i = cache->size - 1; i > 0; i--) {
    if (cache->ranges[i].start_addr < cache->ranges[i - 1].start_addr) {
      addr_range_t temp    = cache->ranges[i];
      cache->ranges[i]     = cache->ranges[i - 1];
      cache->ranges[i - 1] = temp;
    } else {
      break;
    }
  }
}

addr_range_t*
arc_lookup(addr_range_cache_t* cache, uintptr_t addr) {
  // Binary search
  size_t left  = 0;
  size_t right = cache->size - 1;

  while (left <= right) {
    size_t mid = (left + right) / 2;
    if (addr >= cache->ranges[mid].start_addr &&
        addr <= cache->ranges[mid].end_addr) {
      return &cache->ranges[mid];
    } else if (addr < cache->ranges[mid].start_addr) {
      if (mid == 0) break;
      right = mid - 1;
    } else {
      left = mid + 1;
    }
  }

  return nullptr;
}
