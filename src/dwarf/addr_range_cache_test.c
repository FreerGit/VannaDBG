#include "addr_range_cache.h"

#include "domain/libtest.h"

TEST_INIT_GLOBAL();

int
main() {
  int                 num_ranges = 100;
  arena_t             arena      = arena_create(1ULL * 1024 * 1024 * 1024);
  addr_range_cache_t* cache      = arc_create(&arena, num_ranges);

  for (int i = 0; i < num_ranges; i++) {
    addr_range_t line = {i, i + 1, 1};
    arc_insert(cache, line);
  }

  for (int i = 0; i < num_ranges; i++) {
    addr_range_t line = {i, i + 1, 1};
    TEST_ASSERT_EQL(cache->ranges[i].line, line.line);
  }

  for (int i = 0; i < num_ranges; i++) {
    addr_range_t* range = arc_lookup(cache, i);
    TEST_ASSERT(range != nullptr);
  }

  TEST_RESULT();
}