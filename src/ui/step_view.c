#include "step_view.h"

#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>

#include "core/debugger.h"
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

  ui_t *ui  = ((ui_t *)glfwGetWindowUserPointer(window));
  int   pid = ui->dbg->pid;

  if (key == GLFW_KEY_U && action == GLFW_PRESS) {
    if (ptrace(PTRACE_CONT, pid, NULL, NULL) == -1) {
      perror("ptrace(PTRACE_CONT)");
      return;
    }
  }

  if (key == GLFW_KEY_SPACE &&
      (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    // Continue execution
    if (ptrace(PTRACE_CONT, pid, NULL, NULL) == -1) {
      perror("ptrace(PTRACE_CONT)");
      return;
    }

    // step_view_t *view = &((ui_t
    // *)glfwGetWindowUserPointer(window))->step_view; view->curr_stepline =
    // (view->curr_stepline + 1) % view->source_lines.length;
  }
}

ImU32
color_vec4(int r, int g, int b, int a) {
  ImVec4 color = {r, g, b, a};
  return igGetColorU32_Vec4(color);
}

/** r,g,b is 0-255 while a is 0-1 float **/
ImVec4
rgba(uint8_t r, uint8_t g, uint8_t b, float a) {
  return (ImVec4){r / 255.0f, g / 255.0f, b / 255.0f, a};
}

void
step_view_render(step_view_t *view) {
  {
    ImVec2 window_size;
    igGetWindowSize(&window_size);
    igPushStyleColor_Vec4(ImGuiCol_WindowBg, rgba(27, 30, 40, 1.f));

    igBegin("test_files/funcs.c", NULL, 0);
    step_line_t_slice step_lines = view->source_lines;

    for (uint32_t i = 0; i < step_lines.length; i++) {
      ImDrawList  *draw_list = igGetWindowDrawList();
      step_line_t *line      = step_line_t_slice_ref(&step_lines, i);

      ImVec2 cursor_pos;
      igGetCursorScreenPos(&cursor_pos);  // Get current cursor position

      // Define rectangle area
      ImVec2 rect_min = cursor_pos;
      ImVec2 rect_max = {cursor_pos.x + 50,
                         cursor_pos.y + igGetTextLineHeight()};

      // Make the rectangle and text part of an interactive item
      igSetCursorScreenPos(rect_min);
      igInvisibleButton(
          "##rect", (ImVec2){rect_max.x - rect_min.x, rect_max.y - rect_min.y},
          0);

      // Check if the rectangle is hovered
      bool is_hovered = igIsItemHovered(ImGuiHoveredFlags_DelayNone);
      if (igIsItemClicked(0)) {
        line->breakpoint_enabled = !line->breakpoint_enabled;
      }

      // Draw the rectangle with hover effect
      ImU32 rect_color;
      if (line->breakpoint_enabled) {
        rect_color =
            igGetColorU32_Vec4((ImVec4){0, 255, 0, 1});  // Green when clicked
      } else if (is_hovered) {
        rect_color =
            igGetColorU32_Vec4((ImVec4){255, 0, 0, 1});  // Red when hovered
      } else {
        printf("should be void: %s\n", line->source_line);
        rect_color =
            igGetColorU32_Vec4(rgba(166, 172, 205, 1.f));  // Gray when idle
      }
      ImDrawList_AddRectFilled(draw_list, rect_min, rect_max, rect_color, 0.0f,
                               0);

      // Move cursor to the right of the rectangle for text
      igSetCursorScreenPos((ImVec2){
          rect_max.x + 5, cursor_pos.y});  // Adding a small padding of 5 pixels

      // Draw the text
      igText("%d: %s", line->source_num, line->source_line);

      // Submit a dummy item to validate the new extent
      igDummy((ImVec2){window_size.x, igGetTextLineHeightWithSpacing()});
      // Move cursor to the next line
      igSetCursorScreenPos((ImVec2){
          cursor_pos.x, cursor_pos.y + igGetTextLineHeightWithSpacing()});
    }
    igEnd();
    igPopStyleColor(1);
  }
}