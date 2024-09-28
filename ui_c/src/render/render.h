#ifndef RENDER_H
#define RENDER_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "base/base.h"

typedef struct {
  U32 VBO;
  U32 VAO;
} R_Context;

// Call once per program.
// window context has to be intialized before this is called.
R_Context
r_init();

#endif  // RENDER_H