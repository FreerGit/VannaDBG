#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/personality.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "cimgui.h"
#include "cimgui_impl.h"
#include "core/debugger.h"

GLFWwindow *window;

void
graceful_shutdown(GLFWwindow *window) {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  igDestroyContext(NULL);
  glfwDestroyWindow(window);
  glfwTerminate();
}

void
key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  (void)scancode;
  if (key == GLFW_KEY_W && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void
execute_debugee(char *prog_name) {
  if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
    printf("Error in ptrace\n");
    return;
  }
  execl(prog_name, prog_name, NULL);
}

#include "core/registers.h"

void
dump_registers(debugger_t *dbg) {
  for (size_t i = 0; i < n_registers; i++) {
    reg_description_t rd = reg_desc_array[i];
    // printf("%s 0x%016lx\n", rd.name, get_register_value(dbg->pid, rd.r));
    igText("%s 0x%016lx\n", rd.name, get_register_value(dbg->pid, rd.r));
  }
}

int
main() {
  int pid = fork();
  if (pid == 0) {
    personality(ADDR_NO_RANDOMIZE);
    execute_debugee("./test_files/a.out");
  } else if (pid >= 1) {
    printf("Started debugging process %d\n", pid);
    debugger_t dbg = debugger("./test_files/a.out", pid);
    // debugger_run(&dbg);
    // debugger_free(&dbg);

    if (!glfwInit()) return -1;

    // Decide GL+GLSL versions
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    const char *glsl_version = "#version 130";

    // just an extra window hint for resize
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(1024, 768, "VannaDBG", NULL, NULL);
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
    // ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable
    // Gamepad Controls
#ifdef IMGUI_HAS_DOCK
    ioptr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
    ioptr->ConfigFlags |=
        ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform
                                           // Windows
#endif

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    igStyleColorsDark(NULL);

    ImVec4 clearColor;
    clearColor.x = 0.45f;
    clearColor.y = 0.55f;
    clearColor.z = 0.60f;
    clearColor.w = 1.00f;

    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      // start imgui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      igNewFrame();

      glfwSetKeyCallback(window, key_callback);

      // show a simple window that we created ourselves.
      {
        igBegin("Register dump", NULL, 0);
        dump_registers(&dbg);

        // igText("a");
        // igText("b");
        // igText("c");
        // igText("d");
        // igSliderFloat("Float", &f, 0.0f, 1.0f, "%.3f", 0);
        // igColorEdit3("clear color", (float *)&clearColor, 0);

        // ImVec2 buttonSize;
        // buttonSize.x = 0;
        // buttonSize.y = 0;
        // if (igButton("Button", buttonSize)) counter++;
        // igSameLine(0.0f, -1.0f);
        // igText("counter = %d", counter);

        // igText("Application average %.3f ms/frame (%.1f FPS)",
        //        1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
        igEnd();
      }

      // if (showAnotherWindow) {
      //   igBegin("imgui Another Window", &showAnotherWindow, 0);
      //   igText("Hello from imgui");
      //   ImVec2 buttonSize;
      //   buttonSize.x = 0;
      //   buttonSize.y = 0;
      //   if (igButton("Close me", buttonSize)) {
      //     showAnotherWindow = false;
      //   }
      //   igEnd();
      // }

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
  }

  // clean up
  graceful_shutdown(window);

  return 0;
}
