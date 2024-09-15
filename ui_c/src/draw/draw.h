#include "base/base_inc.h"

#ifndef DRAW_H
#define DRAW_H

typedef struct {
  Arena *arena;
} D_ThreadCtx;

// Globals
thread_local D_ThreadCtx *d_thread_ctx = 0;

void
d_being_frame();

#endif  // DRAW_H