#include "step_view.h"

#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ui.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"

SLICE_IMPLEMENTATION(step_line_t);

step_view_t
step_view_load() {
  step_view_t view = {};
  FILE       *fptr = fopen("test_files/funcs.c", "r");
  if (fptr == NULL) {
    assert(0 && "could not open file");
  }

  size_t cap        = 10;
  size_t line_count = 0;
  char   strLine[225];  // assume line length of 225 for now.

  view.source_lines.ptr = malloc(sizeof(step_line_t) * cap);
  while (fgets(strLine, sizeof(strLine), fptr) != NULL) {
    if (line_count == cap) {
      cap *= 2;
      view.source_lines.ptr =
          realloc(view.source_lines.ptr, sizeof(step_line_t) * cap);
    }
    view.source_lines.ptr[line_count] =
        (step_line_t){strdup(strLine), line_count};
    line_count++;
  }

  view.source_lines.length = line_count;

  return view;
}

void
step_view_key_callback(GLFWwindow *window, int key, int scancode, int action,
                       int mods) {
  (void)scancode, (void)mods;
  if (key == GLFW_KEY_SPACE &&
      (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    step_view_t *view = &((ui_t *)glfwGetWindowUserPointer(window))->step_view;
    view->curr_stepline = (view->curr_stepline + 1) % view->source_lines.length;
  }
}

void
step_view_render(step_view_t *view) {
  {
    ImVec2 window_size;
    igGetWindowSize(&window_size);

    igBegin("test_files/funcs.c", NULL, 0);
    step_line_t_slice step_line = view->source_lines;
    for (size_t i = 0; i < step_line.length; i++) {
      const char *text = step_line_t_slice_index(&step_line, i).source_line;
      if (view->curr_stepline == i) {
        ImDrawList *draw_list = igGetWindowDrawList();

        ImVec2 text_pos, text_size;
        igGetCursorScreenPos(&text_pos);
        // printf("L: %s\n", step_line);
        igCalcTextSize(&text_size, text, NULL, false, -1.0f);
        ImVec2 rect_min = text_pos;
        ImVec2 rect_max = {text_pos.x + window_size.x,
                           text_pos.y + text_size.y};
        ImVec4 color    = {255, 0, 0, 255};
        ImDrawList_AddRectFilled(draw_list, rect_min, rect_max,
                                 igGetColorU32_Vec4(color), 0.0f, 0);
      }
      igText("%d: %s", step_line_t_slice_index(&step_line, i).source_num, text);
    }
    igEnd();
  }
}