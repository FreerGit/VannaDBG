#include "free_list.h"

#include "arena.h"
#include "core/breakpoint.h"
#include "libtest.h"

TEST_INIT_GLOBAL()

bool
compare_breakpoint_by_address(const void *item, const void *param) {
  const breakpoint_t *bp   = (const breakpoint_t *)item;
  const intptr_t     *addr = (const intptr_t *)param;
  return bp->addr == *addr;
}

int
main() {
  Arena_t  arena = arena_create(1024 * sizeof(breakpoint_t));
  FreeList free_list;

  free_list_init(&free_list, &arena);

  free_list_create_pool(&free_list, 10, sizeof(breakpoint_t));

  breakpoint_t one = breakpoint(1, 0x55555);

  //   breakpoint_t *found_bp = (breakpoint_t *)free_list_find(
  //       &free_list, compare_breakpoint_by_address, &search_addr);

  //   if (found_bp != NULL) {
  //     printf("Breakpoint found: pid=%d, addr=%lx, enabled=%d\n",
  //     found_bp->pid,
  //            (unsigned long)found_bp->addr, found_bp->enabled);
  //   } else {
  //     printf("Breakpoint with address %lx not found.\n",
  //            (unsigned long)search_addr);
  //   }

  TEST_RESULT();
}