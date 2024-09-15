#include "draw.h"

#include "base/base_inc.c"
#include "base/base_inc.h"

void
d_begin_frame() {
  if (d_thread_ctx == 0) {
    Arena* arena        = arena_alloc(GB(1));
    d_thread_ctx        = push_array(arena, D_ThreadCtx, 1);
    d_thread_ctx->arena = arena;
  }
}
