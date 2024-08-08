#ifndef REGISTER_VIEW_H
#define REGISTER_VIEW_H

#include <GLFW/glfw3.h>
#include <stdint.h>

#include "debugger.h"

typedef struct {
} register_view_t;

register_view_t
register_view_load();

void
register_view_key_callback(GLFWwindow* window, int key, int scancode,
                           int action, int mods);

void
register_view_render(register_view_t* view, debugger_t* dbg);

#endif  // REGISTER_VEW_H