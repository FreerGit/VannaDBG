#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <dwarf.h>
#include <libdwarf.h>
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

  window = glfwCreateWindow(1024, 768, "VannaDBG", NULL, NULL);
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

  FILE *fptr = fopen("test_files/funcs.c", "r");
  if (fptr == NULL) {
    assert(0 && "could not open file");
  }

  size_t cap          = 10;
  size_t line_count   = 0;
  char **source_lines = malloc(sizeof(char *) * cap);
  char   strLine[225];

  while (fgets(strLine, sizeof(strLine), fptr) != NULL) {
    if (line_count == cap) {
      cap *= 2;
      source_lines = realloc(source_lines, sizeof(char *) * cap);
      assert(source_lines != nullptr && "allocation failed");
    }

    source_lines[line_count] = strdup(strLine);
    line_count++;
  }

  // Print the lines to verify

  // free(source_lines[i]);  // Free each line after printing
  // free(source_lines);  // Free the array of pointers

  const long double target_fps     = 144.0;
  const long double frame_duration = 1.0 / target_fps;

  long double last_time = glfwGetTime();
  long double current_time;
  long double elapsed_time;

  while (!glfwWindowShouldClose(window)) {
    current_time = glfwGetTime();
    elapsed_time = current_time - last_time;

    if (elapsed_time < frame_duration) {
      // Sleep for the remaining time to achieve the target FPS
      usleep((frame_duration - elapsed_time) *
             1e6);  // usleep takes microseconds
    }

    last_time = glfwGetTime();

    glfwPollEvents();

    // start imgui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    glfwSetKeyCallback(window, key_callback);

    // show a simple window that we created ourselves.
    {
      igBegin("Register dump", NULL, 0);
      dump_registers(dbg);

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

      igText("%.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate,
             igGetIO()->Framerate);
      igEnd();
    }

    // File display window
    {
      igBegin("test_files/funcs.c", NULL, 0);
      for (size_t i = 0; i < line_count; i++) {
        igText("%s", source_lines[i]);
      }
      igEnd();
    }

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

void
check_error(Dwarf_Error error, const char *msg) {
  if (error != NULL) {
    fprintf(stderr, "Error: %s (%s)\n", msg, dwarf_errmsg(error));
    exit(1);
  }
}

void
print_cu_name(Dwarf_Debug dbg, Dwarf_Die cu_die) {
  char       *name;
  Dwarf_Error error = NULL;
  if (dwarf_diename(cu_die, &name, &error) == DW_DLV_OK) {
    printf("CU Name: %s\n", name);
    dwarf_dealloc(dbg, name, DW_DLA_STRING);
  } else {
    printf("CU Name: <unknown>\n");
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

    int res = start_ui(&dbg);
    return res;

    //   Dwarf_Debug dbg;
    //   Dwarf_Error error = NULL;
    //   int         res;

    //   res = dwarf_init_path("./test_files/a.out", NULL, 0,
    //   DW_GROUPNUMBER_ANY,
    //                         NULL, NULL, &dbg, &error);
    //   if (res != DW_DLV_OK) {
    //     check_error(error, "Failed to initialize DWARF debugging
    //     information");
    //   }

    //   Dwarf_Unsigned     cu_header_length, abbrev_offset, next_cu_header;
    //   Dwarf_Half         version_stamp, address_size;
    //   Dwarf_Die          no_die       = 0, cu_die;
    //   Dwarf_Unsigned     cu_number    = 0;
    //   Dwarf_Line_Context line_context = 0;
    //   Dwarf_Small        table_count  = 0;
    //   Dwarf_Unsigned     lineversion  = 0;
    //   // Dwarf_Error *error

    //   while ((res = dwarf_next_cu_header_d(
    //               dbg, 1, &cu_header_length, &version_stamp, &abbrev_offset,
    //               &address_size, NULL, NULL, NULL, NULL, &next_cu_header,
    //               NULL, &error)) == DW_DLV_OK) {
    //     cu_number++;
    //     printf("\n--- Compilation Unit %llu ---\n", cu_number);
    //     printf("CU Header Length: %llu\n", cu_header_length);
    //     printf("Version Stamp: %hu\n", version_stamp);
    //     printf("Abbrev Offset: %llu\n", abbrev_offset);
    //     printf("Address Size: %hu\n", address_size);

    //     if (dwarf_siblingof_b(dbg, no_die, 1, &cu_die, &error) == DW_DLV_OK)
    //     {
    //       print_cu_name(dbg, cu_die);

    //       // Print CU offset
    //       Dwarf_Off cu_offset;
    //       if (dwarf_dieoffset(cu_die, &cu_offset, &error) == DW_DLV_OK) {
    //         printf("CU Offset: %llu\n", cu_offset);
    //       }

    //       // Print producer (compiler info)
    //       char *producer;
    //       if (dwarf_attr(cu_die, DW_AT_producer, &producer, &error) ==
    //           DW_DLV_OK) {
    //         printf("Producer: %s\n", producer);
    //         dwarf_dealloc(dbg, producer, DW_DLA_STRING);
    //       }

    //       // Print language
    //       Dwarf_Half language;
    //       if (dwarf_srclang(cu_die, &language, &error) == DW_DLV_OK) {
    //         printf("Language: %hu\n", language);
    //       }

    //       // Print line number info
    //       Dwarf_Line  *linebuf   = NULL;
    //       Dwarf_Signed linecount = 0;
    //       int          sres      = 0;
    //       if (dwarf_srclines_b(cu_die, &lineversion, &table_count,
    //       &line_context,
    //                            &error) == DW_DLV_OK) {
    //         printf("Number of lines: %hhu\n", table_count);
    //         Dwarf_Signed i          = 0;
    //         Dwarf_Signed baseindex  = 0;
    //         Dwarf_Signed file_count = 0;
    //         Dwarf_Signed endindex   = 0;
    //         sres = dwarf_srclines_files_indexes(line_context, &baseindex,
    //                                             &file_count, &endindex,
    //                                             error);
    //         if (sres != DW_DLV_OK) {
    //           /* Something badly wrong! */
    //           return sres;
    //         }

    //         sres = dwarf_srclines_from_linecontext(line_context, &linebuf,
    //                                                &linecount, error);
    //         if (sres != DW_DLV_OK) {
    //           /* Error. Clean up the context information. */
    //           dwarf_srclines_dealloc_b(line_context);
    //           return sres;
    //         }
    //         /* The lines are normal line table lines. */
    //         for (i = 0; i < linecount; ++i) {
    //           /* use linebuf[i] */
    //           Dwarf_Unsigned column;
    //           if (dwarf_lineno(linebuf[i], &column, &error) == DW_DLV_OK) {
    //             printf("Column: %llu\n", (unsigned long long)column);
    //           }
    //           // printf("%d", );
    //         }
    //         // print_line_info(dbg, cu_die);
    //         dwarf_dealloc(dbg, linebuf, DW_DLA_LIST);
    //       }

    //       dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
    //     }
    //   }

    //   if (res == DW_DLV_ERROR) {
    //     check_error(error, "Error in dwarf_next_cu_header_d");
    //   }

    //   // int res = 0;

    //   return res;
  }
}