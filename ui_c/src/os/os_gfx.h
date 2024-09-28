
#include "base/base_inc.h"

#ifndef OS_GFX_H
#define OS_GFX_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct {
  GLFWwindow* handle;
} Window;

// Init API
void
os_gfx_init();

// Windows
Window
os_window_open(Vec2F32 resolution, String8 title);

bool
os_window_should_close(Window* window);

#endif  // OS_GFX_H