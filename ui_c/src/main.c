#include <stdbool.h>
#include <stdio.h>

#include "os/os_inc.c"
#include "os/os_inc.h"
#include "render/render.c"
#include "render/render.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// typedef struct {
//   float x, y, width, height;
//   float r, g, b;
//   bool  clicked;
// } Button;

// Button
// createButton(float x, float y, float  width, float height, float r, float g,
//              float b) {
//   Button btn = {x, y, width, height, r, g, b, false};
//   return btn;
// }

// bool
// isPointInRect(float x, float y, float rx, float ry, float rw, float rh) {
//   return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
// }

// void
// drawRect(float x, float y, float width, float height, float r, float g,
//          float b) {
//   glColor3f(r, g, b);
//   glBegin(GL_QUADS);
//   glVertex2f(x, y);
//   glVertex2f(x + width, y);
//   glVertex2f(x + width, y + height);
//   glVertex2f(x, y + height);
//   glEnd();
// }

// void
// drawButton(Button* btn) {
//   drawRect(btn->x, btn->y, btn->width, btn->height, btn->r, btn->g, btn->b);
// }

// void
// mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
//   if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
//     double xpos, ypos;
//     glfwGetCursorPos(window, &xpos, &ypos);
//     Button* btn = (Button*)glfwGetWindowUserPointer(window);

//     if (isPointInRect(xpos, ypos, btn->x, btn->y, btn->width, btn->height)) {
//       btn->clicked = true;
//       btn->r       = 1.0f - btn->r;
//       btn->g       = 1.0f - btn->g;
//       btn->b       = 1.0f - btn->b;
//     }
//   }
// }

int
main() {
  os_gfx_init();

  Vec2F32 resolution = {.x = 800, .y = 600};

  String8 title = str8_lit_comp("Custom UI thing");
  Window window = os_window_open(resolution, title);

  render_init();

  double lastTime = glfwGetTime();
  int nbFrames = 0;

  // Button btn = createButton(300, 250, 200, 100, 0.2f, 0.6f, 1.0f);
  // glfwSetWindowUserPointer(window.handle, &btn);
  // glfwSetMouseButtonCallback(window.handle, mouseButtonCallback);

  for (U8 quit = 0; quit == 0;) {
    double currentTime = glfwGetTime();
    nbFrames++;
    if (currentTime - lastTime >=
        1.0) {  // If last print was more than 1 sec ago
      printf("%f FPS\n", (double)nbFrames / (currentTime - lastTime));
      nbFrames = 0;
      lastTime += 1.0;
    }

    // TODO(work) handle resizing of window
    // int width, height;
    // glfwGetFramebufferSize(window.handle, &width, &height);
    // glViewport(0, 0, width, height);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glClear(GL_COLOR_BUFFER_BIT);
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    // glOrtho(0, width, height, 0, -1, 1);
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();

    // drawButton(&btn);

    glfwSwapBuffers(window.handle);
    // glfwPollEvents();

    quit = os_window_should_close(&window);
  }

  glfwDestroyWindow(window.handle);
  glfwTerminate();
  return 0;
}