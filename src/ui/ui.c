#include "ui.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <unistd.h>

#include "core/debugger.h"
// #include "core/registers.h"
#include "register_view.h"
#include "step_view.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "cimgui_impl.h"

void
graceful_shutdown(GLFWwindow *window) {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  igDestroyContext(NULL);
  glfwDestroyWindow(window);
  glfwTerminate();
}

void
handle_key_callbacks(GLFWwindow *window, int key, int scancode, int action,
                     int mods) {
  (void)scancode;
  if (key == GLFW_KEY_W && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  step_view_key_callback(window, key, scancode, action, mods);
  register_view_key_callback(window, key, scancode, action, mods);
}

int
start_ui(debugger_t *dbg) {
  if (!glfwInit()) return -1;

  // Decide GL+GLSL versions
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  const char *glsl_version = "#version 130";

  // just an extra window hint for resize
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

  GLFWwindow *window = glfwCreateWindow(1024, 768, "VannaDBG", NULL, NULL);
  if (!window) {
    printf("Failed to create window! Terminating!\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  // enable vsync
  glfwSwapInterval(0);

  // setup imgui
  igCreateContext(NULL);

  // set docking
  ImGuiIO *ioptr = igGetIO();
  ioptr->ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
#ifdef IMGUI_HAS_DOCK
  ioptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
  // Enable for Multi-Viewport / Platform
  // ioptr->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  igStyleColorsDark(NULL);

  ImVec4 clearColor;
  clearColor.x = 0.45f;
  clearColor.y = 0.55f;
  clearColor.z = 0.60f;
  clearColor.w = 1.00f;

  step_view_t     step_view     = step_view_load();
  register_view_t register_view = register_view_load();
  ui_t            ui_context    = (ui_t){dbg, step_view, register_view};

  // create lines and set mimic stepping (set first)
  // lines[0].highlighted = true;

  // Print the lines to verify

  // free(source_lines[i]);  // Free each line after printing
  // free(source_lines);  // Free the array of pointers

  glfwSetWindowUserPointer(window, &ui_context);
  glfwSetKeyCallback(window, handle_key_callbacks);

  // step_view_init(window);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // start imgui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    // show a simple window that we created ourselves.
    register_view_render(&ui_context.register_view, ui_context.dbg);
    // File display window

    step_view_render(&ui_context.step_view, ui_context.dbg);

    // render
    igRender();
    glfwMakeContextCurrent(window);
    glViewport(0, 0, (int)ioptr->DisplaySize.x, (int)ioptr->DisplaySize.y);
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
#ifdef IMGUI_HAS_DOCK
    if (ioptr->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow *backup_current_window = glfwGetCurrentContext();
      igUpdatePlatformWindows();
      igRenderPlatformWindowsDefault(NULL, NULL);
      glfwMakeContextCurrent(backup_current_window);
    }
#endif
    glfwSwapBuffers(window);
  }

  // clean up
  graceful_shutdown(window);
  return 0;
}