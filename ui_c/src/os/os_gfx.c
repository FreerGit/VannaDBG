#include "os_gfx.h"

#include <assert.h>

void
os_gfx_init() {
  assert(glfwInit());
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

  glewExperimental = GL_TRUE;
  assert(glewInit() == GLEW_OK && "Could not create glew bindings");

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