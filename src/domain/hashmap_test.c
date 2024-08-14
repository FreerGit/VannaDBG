#include "hashmap.h"

#include <time.h>

#include "breakpoint.h"
#include "libtest.h"

TEST_INIT_GLOBAL();

// You could use the uintptr_t directly but we try to get better bucket
// distribution.
size_t
uintptr_hash(const void *key) {
  uintptr_t value = *(const uintptr_t *)key;
  value ^= (value >> 32);
  return (size_t)value;
}

static int comparisons = 0;

bool
uintptr_equal(const void *key1, const void *key2) {
  comparisons++;
  return *(const uintptr_t *)key1 == *(const uintptr_t *)key2;
}

int
main() {
  arena_t    arena = arena_create(1024 * 1024);
  hashmap_t *map   = hashmap_create(&arena, 100, sizeof(breakpoint_t),
                                    uintptr_hash, uintptr_equal);

  uintptr_t TOTAL_BPS = 100;
  for (uintptr_t i = 0; i < TOTAL_BPS; i++) {
    breakpoint_t bp = (breakpoint_t){.pid = i, .addr = 0, .enabled = true};
    hashmap_put(map, &i, &bp, sizeof(uintptr_t));
  }

  for (int i = 0; i < (int)TOTAL_BPS; i++) {
    breakpoint_t *found_bp = (breakpoint_t *)hashmap_get(map, &i);
    TEST_ASSERT(found_bp->enabled && found_bp->pid == i);
  }

  uintptr_t addr = 55;
  hashmap_remove(map, &addr);
  breakpoint_t *bp = (breakpoint_t *)hashmap_get(map, &addr);
  TEST_ASSERT_EQL(bp, NULL);

  addr = 65;
  hashmap_remove(map, &addr);
  bp = (breakpoint_t *)hashmap_get(map, &addr);
  TEST_ASSERT_EQL(bp, NULL);

  arena_destroy(&arena);
  TEST_RESULT();
}