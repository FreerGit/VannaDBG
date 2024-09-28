#include "render.h"

#include <assert.h>

// Vertex Shader
const char* vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "}\0";

// Fragment Shader (Dynamic color via uniform)
const char* fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 frag_shader_color;\n"
    "void main() {\n"
    "   FragColor = frag_shader_color;\n"
    "}\n\0";

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

  R_Context ctx = {0};
  glGenVertexArrays(1, &ctx.VAO);
  glGenBuffers(1, &ctx.VBO);

  glBindVertexArray(ctx.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, ctx.VBO);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertic))
}