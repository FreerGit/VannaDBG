#include "render.h"

#include <assert.h>

// Vertex Shader
const char* vertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 position;\n"
    "layout(location = 1) in vec3 color;\n"
    "out vec3 vertexColor;\n"
    "void main() {\n"
    "    gl_Position = vec4(position, 1.0);\n"
    "    vertexColor = color;\n"
    "}\0";

// Fragment Shader (Dynamic color via uniform)
const char* fragmentShaderSource =
    "#version 330 core\n"
    "in vec3 vertexColor;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    fragColor = vec4(vertexColor, 1.0);\n"
    "}\n\0";

// TODO cleanup
R_Context
r_init() {
  // Build and compile the vertex shader
  int          success;
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  assert(success && "Could not build/compile vertex shader");

  // Build and compile the fragment shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  assert(success && "Could not build/compile fragment shader");

  // Link the shaders
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

  // We can safely remove them after the linking
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  R_Context ctx      = {0};
  ctx.shader_program = shaderProgram;
  glGenVertexArrays(1, &ctx.VAO);
  glGenBuffers(1, &ctx.VBO);

  glBindVertexArray(ctx.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, ctx.VBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return ctx;
}

void
render(R_Context ctx, F32* vertices, U64 vert_num) {
  glUseProgram(ctx.shader_program);
  glBindVertexArray(ctx.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, ctx.VBO);
  glBufferData(GL_ARRAY_BUFFER, vert_num * sizeof(float), vertices,
               GL_DYNAMIC_DRAW);
  glDrawArrays(GL_TRIANGLES, 0,
               vert_num / 6);  // Assuming 3 position and 3 color components
  glBindVertexArray(0);
}
