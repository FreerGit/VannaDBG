#include "ui.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "debugger.h"
#include "registers.h"
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
}

void
execute_debugee(char *prog_name) {
  if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
    printf("Error in ptrace\n");
    return;
  }
  execl(prog_name, prog_name, NULL);
}

void
dump_registers(debugger_t *dbg) {
  for (size_t i = 0; i < n_registers; i++) {
    reg_description_t rd = reg_desc_array[i];
    // printf("%s 0x%016lx\n", rd.name, get_register_value(dbg->pid, rd.r));
    igText("%s 0x%016lx\n", rd.name, get_register_value(dbg->pid, rd.r));
  }
}

// void
// check_error(Dwarf_Error error, const char *msg) {
//   if (error != NULL) {
//     fprintf(stderr, "Error: %s (%s)\n", msg, dwarf_errmsg(error));
//     exit(1);
//   }
// }

// void
// print_cu_name(Dwarf_Debug dbg, Dwarf_Die cu_die) {
//   char       *name;
//   Dwarf_Error error = NULL;
//   if (dwarf_diename(cu_die, &name, &error) == DW_DLV_OK) {
//     printf("CU Name: %s\n", name);
//     dwarf_dealloc(dbg, name, DW_DLA_STRING);
//   } else {
//     printf("CU Name: <unknown>\n");
//   }
// }

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
  glfwSwapInterval(1);

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

  step_view_t step_view = step_view_load();
  // create lines and set mimic stepping (set first)
  // lines[0].highlighted = true;

  // Print the lines to verify

  // free(source_lines[i]);  // Free each line after printing
  // free(source_lines);  // Free the array of pointers

  glfwSetWindowUserPointer(window, &step_view);
  glfwSetKeyCallback(window, handle_key_callbacks);

  // step_view_init(window);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // start imgui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    // show a simple window that we created ourselves.
    {
      igBegin("Register dump", NULL, 0);
      dump_registers(dbg);
      igText("%.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate,
             igGetIO()->Framerate);
      igEnd();
    }

    // File display window
    step_view_render(&step_view);

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