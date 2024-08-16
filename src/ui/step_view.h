#ifndef STEP_VIEW_H
#define STEP_VIEW_H

#include <GLFW/glfw3.h>
#include <stdint.h>

#include "core/debugger.h"
#include "domain/slice.h"

typedef struct {
  char* source_line;
  int   source_num;
  bool  breakpoint_enabled;
} step_line_t;

SLICE_DEFINITION(step_line_t);

typedef struct {
  step_line_t_slice source_lines;
} step_view_t;

step_view_t
step_view_load();

void
step_view_key_callback(GLFWwindow* window, int key, int scancode, int action,
                       int mods);

void
step_view_render(step_view_t* view, debugger_t* dbg);

#endif  // STEP_VEW_H