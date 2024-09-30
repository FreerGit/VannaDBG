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
// createButton(float x, float y, float  width, float height, float r, float
// g,
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
//   drawRect(btn->x, btn->y, btn->width, btn->height, btn->r, btn->g,
// btn->b);
// }

// void
// mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
// {
//   if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
//     double xpos, ypos;
//     glfwGetCursorPos(window, &xpos, &ypos);
//     Button* btn = (Button*)glfwGetWindowUserPointer(window);

//     if (isPointInRect(xpos, ypos, btn->x, btn->y, btn->width,
// btn->height)) {
//       btn->clicked = true;
//       btn->r       = 1.0f - btn->r;
//       btn->g       = 1.0f - btn->g;
//       btn->b       = 1.0f - btn->b;
//     }
//   }
// }

void
print_fps() {
  static double last_time   = 0.0;
  static int    frame_count = 0;

  double time = glfwGetTime();
  frame_count++;

  if (time - last_time >= 1.0) {  // If a second has passed
    printf("FPS: %d\n", frame_count);
    frame_count = 0;
    last_time   = time;
  }
}

int
main() {
  os_gfx_init();

  Vec2F32 resolution = {.x = 800, .y = 600};

  String8 title  = str8_lit_comp("Custom UI thing");
  Window  window = os_window_open(resolution, title);

  R_Context r_handle = r_init();

  // Define vertices for a triangle
  float triangleVertices[] = {
      // Positions        // Colors
      0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  // Vertex 1 (Red)
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // Vertex 2 (Green)
      0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f   // Vertex 3 (Blue)
  };

  // Define vertices for a square
  float squareVertices[] = {
      // Positions        // Colors
      -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  // Top-left (Red)
      0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f,  // Top-right (Green)
      0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  // Bottom-right (Blue)

      -0.5f, 0.5f,  0.0f, 1.0f, 0.0f, 0.0f,  // Top-left (Red)
      0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  // Bottom-right (Blue)
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f   // Bottom-left (Green)
  };

  for (U8 quit = 0; quit == 0;) {
    print_fps();
    glClear(GL_COLOR_BUFFER_BIT);

    // // Render triangle
    // render(r_handle, triangleVertices,
    //        sizeof(triangleVertices) / sizeof(triangleVertices[0]));

    // // Render square
    render(r_handle, squareVertices,
           sizeof(squareVertices) / sizeof(squareVertices[0]));

    glfwSwapBuffers(window.handle);
    glfwPollEvents();

    quit = os_window_should_close(&window);
  }

  glfwDestroyWindow(window.handle);
  glfwTerminate();
  return 0;
}

// #include <GL/glew.h>
// #include <GLFW/glfw3.h>
// #include <stdio.h>
// #include <stdlib.h>

// // Vertex Shader
// const char* vertexShaderSource =
//     "#version 330 core\n"
//     "layout (location = 0) in vec3 aPos;\n"
//     "void main() {\n"
//     "   gl_Position = vec4(aPos, 1.0);\n"
//     "}\0";

// // Fragment Shader (Dynamic color via uniform)
// const char* fragmentShaderSource =
//     "#version 330 core\n"
//     "out vec4 FragColor;\n"
//     "void main() {\n"
//     "   FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
//     "}\n\0";

// // Function to compile shaders
// GLuint
// loadShader(GLenum type, const char* source) {
//   GLuint shader = glCreateShader(type);
//   glShaderSource(shader, 1, &source, NULL);
//   glCompileShader(shader);

//   // Check for compile errors
//   GLint success;
//   glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
//   if (!success) {
//     char infoLog[512];
//     glGetShaderInfoLog(shader, 512, NULL, infoLog);
//     fprintf(stderr, "Shader compilation error: %s\n", infoLog);
//     exit(EXIT_FAILURE);
//   }
//   return shader;
// }

// // Function to create the shader program
// GLuint
// createShaderProgram() {
//   GLuint vertexShader   = loadShader(GL_VERTEX_SHADER, vertexShaderSource);
//   GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER,
//   fragmentShaderSource);

//   GLuint shaderProgram = glCreateProgram();
//   glAttachShader(shaderProgram, vertexShader);
//   glAttachShader(shaderProgram, fragmentShader);
//   glLinkProgram(shaderProgram);

//   // Check for linking errors
//   GLint success;
//   glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
//   if (!success) {
//     char infoLog[512];
//     glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
//     fprintf(stderr, "Program linking error: %s\n", infoLog);
//     exit(EXIT_FAILURE);
//   }

//   glDeleteShader(vertexShader);
//   glDeleteShader(fragmentShader);
//   return shaderProgram;
// }

// int
// main() {
//   // Initialize GLFW
//   if (!glfwInit()) {
//     fprintf(stderr, "Failed to initialize GLFW\n");
//     return -1;
//   }

//   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//   glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

//   // Create a windowed mode window and its OpenGL context
//   GLFWwindow* window =
//       glfwCreateWindow(800, 600, "OpenGL Triangle", NULL, NULL);
//   if (!window) {
//     fprintf(stderr, "Failed to create GLFW window\n");
//     glfwTerminate();
//     return -1;
//   }
//   glfwMakeContextCurrent(window);

//   glfwSwapInterval(0);

//   // Initialize GLEW
//   glewExperimental = GL_TRUE;
//   if (glewInit() != GLEW_OK) {
//     fprintf(stderr, "Failed to initialize GLEW\n");
//     return -1;
//   }

//   // Set up vertex data
//   float vertices[] = {
//       // Triangle vertices
//       -0.5f, -0.5f, 0.0f,  // Bottom left
//       0.5f,  -0.5f, 0.0f,  // Bottom right
//       0.0f,  0.5f,  0.0f   // Top
//   };

//   // Create VBO and VAO
//   GLuint VBO, VAO;
//   glGenVertexArrays(1, &VAO);
//   glGenBuffers(1, &VBO);

//   glBindVertexArray(VAO);

//   glBindBuffer(GL_ARRAY_BUFFER, VBO);
//   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
//   GL_STATIC_DRAW);

//   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
//   (void*)0); glEnableVertexAttribArray(0);

//   glBindBuffer(GL_ARRAY_BUFFER, 0);
//   glBindVertexArray(0);

//   // Create shader program
//   GLuint shaderProgram = createShaderProgram();

//   // Main loop
//   while (!glfwWindowShouldClose(window)) {
//     // Clear the color buffer
//     glClear(GL_COLOR_BUFFER_BIT);

//     // Use the shader program
//     glUseProgram(shaderProgram);

//     // Draw the triangle
//     glBindVertexArray(VAO);
//     glDrawArrays(GL_TRIANGLES, 0, 3);

//     calculateFPS();
//     // Swap buffers and poll for events
//     glfwSwapBuffers(window);
//     glfwPollEvents();
//   }

//   // Clean up
//   glDeleteVertexArrays(1, &VAO);
//   glDeleteBuffers(1, &VBO);
//   glDeleteProgram(shaderProgram);

//   glfwTerminate();
//   return 0;
// }