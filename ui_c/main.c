#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct {
  float x, y, width, height;
  float r, g, b;
  bool  clicked;
} Button;

Button
createButton(float x, float y, float width, float height, float r, float g,
             float b) {
  Button btn = {x, y, width, height, r, g, b, false};
  return btn;
}

bool
isPointInRect(float x, float y, float rx, float ry, float rw, float rh) {
  return x >= rx && x <= rx + rw && y >= ry && y <= ry + rh;
}

void
drawRect(float x, float y, float width, float height, float r, float g,
         float b) {
  glColor3f(r, g, b);
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + width, y);
  glVertex2f(x + width, y + height);
  glVertex2f(x, y + height);
  glEnd();
}

void
drawButton(Button* btn) {
  drawRect(btn->x, btn->y, btn->width, btn->height, btn->r, btn->g, btn->b);
}

void
mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    Button* btn = (Button*)glfwGetWindowUserPointer(window);

    if (isPointInRect(xpos, ypos, btn->x, btn->y, btn->width, btn->height)) {
      btn->clicked = true;
      btn->r       = 1.0f - btn->r;
      btn->g       = 1.0f - btn->g;
      btn->b       = 1.0f - btn->b;
    }
  }
}

int
main() {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  GLFWwindow* window = glfwCreateWindow(
      WINDOW_WIDTH, WINDOW_HEIGHT, "Immediate Mode UI Example", NULL, NULL);
  if (!window) {
    fprintf(stderr, "Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);

  double lastTime = glfwGetTime();
  int    nbFrames = 0;

  Button btn = createButton(300, 250, 200, 100, 0.2f, 0.6f, 1.0f);
  glfwSetWindowUserPointer(window, &btn);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);

  while (!glfwWindowShouldClose(window)) {
    double currentTime = glfwGetTime();
    nbFrames++;
    if (currentTime - lastTime >=
        1.0) {  // If last print was more than 1 sec ago
      printf("%f FPS\n", (double)nbFrames / (currentTime - lastTime));
      nbFrames = 0;
      lastTime += 1.0;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawButton(&btn);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}