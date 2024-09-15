#include "os_gfx.h"

#include <GLFW/glfw3.h>
#include <assert.h>

void
os_gfx_init() {
  assert(glfwInit());
}

Window
os_window_open(Vec2F32 resolution, String8 title) {
  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  GLFWwindow* window = glfwCreateWindow(resolution.x, resolution.y,
                                        (const char*)title.str, NULL, NULL);
  if (!window) {
    glfwTerminate();
    assert(window && "Could not create window");
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);
  return (Window){.handle = window};
}

bool
os_window_should_close(Window* window) {
  bool should_close =
      (glfwGetKey(window->handle, GLFW_KEY_ESCAPE) == GLFW_PRESS);
  if (should_close) {
    glfwSetWindowShouldClose(window->handle, 1);
  }
  return should_close;
}