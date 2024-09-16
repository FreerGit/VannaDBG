#include "base/base_inc.c"
#include "base/base_testlib.h"

TEST_INIT_GLOBAL();

typedef struct {
  U64 u;
  S32 s;
  F64 d;
} Medium;

S32
main() {
  Arena arena = arena_alloc(1024);
  TEST_ASSERT(arena.capacity >= 1024);  // Align to page size
  TEST_ASSERT(arena.offset == 0);

  arena_push(&arena, sizeof(Medium));
  arena_push(&arena, sizeof(Medium));
  arena_push(&arena, sizeof(Medium));
  TEST_ASSERT(arena_pos(&arena) == 24 * 3);

  arena_pop(&arena, sizeof(Medium));
  TEST_ASSERT(arena_pos(&arena) == 24 * 2);

  arena_reset(&arena);
  TEST_ASSERT(arena_pos(&arena) == 0);

  // Scratch Tests
  // Set some arbitrary unaligned number to enfore correct popping
  arena_push(&arena, 15);
  TEST_ASSERT(arena_pos(&arena) == 16);  // Auto align to uintptr_t (8)

  arena_reset(&arena);

  Scratch scratch = arena_scratch_begin(&arena);
  arena_push(scratch.arena, sizeof(Medium));
  arena_push(scratch.arena, sizeof(Medium));
  arena_push(scratch.arena, sizeof(Medium));
  arena_push(scratch.arena, sizeof(Medium));
  arena_pop(scratch.arena, sizeof(Medium));

  TEST_ASSERT(arena_pos(scratch.arena) == (sizeof(Medium) * 3));
  arena_scratch_end(scratch);

  // ensure that after scratch is done, we are back to original offset.
  TEST_ASSERT(arena_pos(scratch.arena) == 0);

  // Macros
  Scratch scratch_arr = arena_scratch_begin(&arena);
  push_array(scratch_arr.arena, Medium, 42);

  TEST_ASSERT(arena_pos(scratch_arr.arena) == sizeof(Medium) * 42);

  arena_scratch_end(scratch);

  // Release the arena, can't use it anymore.
  arena_release(&arena);
  TEST_ASSERT(arena.capacity == 0);
  TEST_ASSERT(arena.offset == 0);

  TEST_RESULT();
}