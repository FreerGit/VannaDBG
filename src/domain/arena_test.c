#include "arena.h"

#include "libtest.h"
TEST_INIT_GLOBAL();

int
main() {
  size_t  arena_size = 8ULL * 1024 * 1024 * 1024;  // 8 GB
  arena_t arena      = arena_create(arena_size);

  arena_alloc(&arena, 100 * sizeof(int));

  arena_t scratch =
      arena_scratch(&arena, 1ULL * 1024 * 1024 * 1024);  // 1 GB temp

  size_t *temp_data = (size_t *)arena_alloc(&scratch, 500 * sizeof(size_t));

  for (size_t i = 0; i < 500; i++) {
    temp_data[i] = i;
  }

  arena_reset(&scratch);

  size_t *overwritten = (size_t *)arena_alloc(&scratch, 300 * sizeof(size_t));

  for (size_t i = 0; i < 300; i++) {
    overwritten[i] = i + 42;
  }

  // We have overwritten 300 elements in the temp arena, the 200 after which
  // should still be there from the previous allocation & for-loop
  for (size_t i = 0; i < 500; i++) {
    if (i < 300) {
      TEST_ASSERT(overwritten[i] == i + 42);
    } else {
      TEST_ASSERT(overwritten[i] == i);
    }
  }

  TEST_ASSERT_EQL(scratch.offset, 300 * sizeof(size_t));

  // Reset the memory and destory the arena (you can of course just skip
  // arena_reset here)
  arena_reset(&arena);
  arena_destroy(&arena);

  TEST_RESULT();
}