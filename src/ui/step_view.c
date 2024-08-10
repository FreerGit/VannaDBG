#include "step_view.h"

#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>

#include "debugger.h"
#include "dwarf.h"
#include "libdwarf.h"
#include "ui.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"

SLICE_IMPLEMENTATION(step_line_t);

step_view_t
step_view_load() {
  step_view_t view = {};
  FILE       *fptr = fopen("test_files/funcs.c", "r");
  if (fptr == NULL) {
    assert(0 && "could not open file");
  }

  size_t cap        = 10;
  size_t line_count = 0;
  char   strLine[225];  // assume line length of 225 for now.

  view.source_lines.ptr = malloc(sizeof(step_line_t) * cap);
  while (fgets(strLine, sizeof(strLine), fptr) != NULL) {
    if (line_count == cap) {
      cap *= 2;
      view.source_lines.ptr =
          realloc(view.source_lines.ptr, sizeof(step_line_t) * cap);
    }
    view.source_lines.ptr[line_count] =
        (step_line_t){strdup(strLine), line_count};
    line_count++;
  }

  view.source_lines.length = line_count;

  // P

  return view;
}

void
map_ip_to_source(debugger_t *dbg, Dwarf_Addr ip) {
}

int
get_line_from_pc(Dwarf_Debug dbg, Dwarf_Addr pc, char **filename,
                 Dwarf_Unsigned *line_number) {
  Dwarf_Die          cu_die       = 0;
  Dwarf_Line        *lines        = 0;
  Dwarf_Signed       line_count   = 0;
  Dwarf_Addr         lineaddr     = 0;
  Dwarf_Error        error        = 0;
  int                result       = DW_DLV_ERROR;
  Dwarf_Unsigned     lineno       = 0;
  Dwarf_Unsigned     version      = 0;
  Dwarf_Small        table_count  = 0;
  Dwarf_Line_Context line_context = 0;

  printf("Searching for PC: 0x%llx\n", (unsigned long long)pc);

  while (dwarf_next_cu_header_d(dbg, 1, NULL, NULL, NULL, NULL, NULL, NULL,
                                NULL, NULL, NULL, NULL, &error) == DW_DLV_OK) {
    printf("Processing new CU\n");

    if (dwarf_siblingof_b(dbg, 0, 1, &cu_die, &error) == DW_DLV_OK) {
      printf("Got sibling of CU\n");
      if (dwarf_srclines_b(cu_die, &version, &table_count, &line_context,
                           &error) == DW_DLV_OK) {
        printf("Got source lines. Version: %llu, Table count: %u\n", version,
               table_count);
        if (dwarf_srclines_from_linecontext(line_context, &lines, &line_count,
                                            &error) == DW_DLV_OK) {
          printf("Line count: %lld\n", line_count);
          for (int i = 0; i < line_count; ++i) {
            if (dwarf_lineaddr(lines[i], &lineaddr, &error) != DW_DLV_OK) {
              printf("Failed to get line address for line %d\n", i);
              continue;
            }

            printf("Line %d address: 0x%llx\n", i, lineaddr);

            if (lineaddr > pc) {
              printf("Found line after PC\n");
              if (i > 0) {
                Dwarf_Line prev_line = lines[i - 1];
                char      *src_file  = NULL;
                if (dwarf_lineno(prev_line, &lineno, &error) == DW_DLV_OK &&
                    dwarf_linesrc(prev_line, &src_file, &error) == DW_DLV_OK) {
                  *line_number = lineno;
                  *filename    = src_file;

                  printf("Match found!\n");
                  printf("Source file: %s\n", src_file);
                  printf("Line number: %llu\n", lineno);
                  printf("Instruction pointer: 0x%llx\n",
                         (unsigned long long)pc);

                  result = DW_DLV_OK;
                  goto cleanup;
                }
              }
              break;
            }
          }
          dwarf_srclines_dealloc_b(line_context);
        } else {
          printf("Failed to get lines from line context\n");
        }
      } else {
        printf("Failed to get source lines\n");
      }
    } else {
      printf("Failed to get sibling of CU\n");
    }
    if (cu_die) {
      dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
      cu_die = 0;
    }
  }

  printf("No matching line found for PC: 0x%llx\n", (unsigned long long)pc);

cleanup:
  if (cu_die) {
    dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
  }
  if (line_context) {
    dwarf_srclines_dealloc_b(line_context);
  }
  if (error) {
    dwarf_dealloc(dbg, error, DW_DLA_ERROR);
  }
  return result;
}

// uintptr_t
// get_base_address(int pid) {
//   char path[256];
//   sprintf(path, "/proc/%d/maps", pid);
//   FILE *fp = fopen(path, "r");
//   assert(fp != NULL);

//   uintptr_t base_address = 0;
//   char      line[256];

//   if (fgets(line, sizeof(line), fp) != NULL) {
//     sscanf(line, "%lx-%*lx", &base_address);
//   }

//   fclose(fp);
//   return base_address;
// }

// Dwarf_Addr
// get_base_address(pid_t pid) {
//   char filename[64];
//   snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);

//   FILE *fp = fopen(filename, "r");
//   if (fp == NULL) {
//     perror("Failed to open maps file");
//     return 0;
//   }

//   char       line[256];
//   Dwarf_Addr base_addr = 0;

//   while (fgets(line, sizeof(line), fp)) {
//     unsigned long long start, end;
//     char               permissions[5];
//     char               path[256];

//     // Parse the line
//     int matched = sscanf(line, "%llx-%llx %4s %*x %*x:%*x %*d %255s", &start,
//                          &end, permissions, path);

//     // We're looking for the executable segment of our main program
//     if (matched == 4 && strstr(permissions, "x") && strstr(path, "a.out")) {
//       base_addr = start;
//       break;
//     }
//   }

//   fclose(fp);
//   return base_addr;
// }

void
step_view_key_callback(GLFWwindow *window, int key, int scancode, int action,
                       int mods) {
  (void)scancode, (void)mods;

  ui_t *ui  = ((ui_t *)glfwGetWindowUserPointer(window));
  int   pid = ui->dbg->pid;

  if (key == GLFW_KEY_U && action == GLFW_PRESS) {
    if (ptrace(PTRACE_CONT, pid, NULL, NULL) == -1) {
      perror("ptrace(PTRACE_CONT)");
      return;
    }
  }

  if (key == GLFW_KEY_SPACE &&
      (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    struct user_regs_struct regs;

    Dwarf_Addr base_addr = 0x555555554000;
    Dwarf_Addr main_addr = 0x555555555189;
    // find_main_address(ui->dbg->dwarf, base_addr);
    printf("main: %llx\n", main_addr);
    long data = ptrace(PTRACE_PEEKTEXT, pid, (void *)main_addr, NULL);
    if ((data & 0xFF) != 0xCC) {
      printf("Breakpoint not set correctly at 0x%llx\n", main_addr);
      return;
    }

    printf("base addr: %llx\nmain addr: %llx\n", base_addr, main_addr);
    if (main_addr == 0) {
      fprintf(stderr, "Failed to find main function\n");
      return;
    }

    // Continue execution
    if (ptrace(PTRACE_CONT, pid, NULL, NULL) == -1) {
      perror("ptrace(PTRACE_CONT)");
      return;
    }

    // Wait for the child to stop
    int status;
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid");
      return;
    }

    if (WIFEXITED(status)) {
      printf("Child exited\n");
      return;
    }

    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
      printf("fdsa\n");
      // Get the current registers
      if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1) {
        perror("ptrace(PTRACE_GETREGS)");
        return;
      }
      // Confirm we're at main
      if (regs.rip - 1 == main_addr) {  // -1 because rip will be after the
                                        // breakpoint instruction
        printf("Reached main function at address: 0x%llx\n",
               (unsigned long long)main_addr);
        printf("Current instruction pointer: 0x%llx\n",
               (unsigned long long)regs.rip);
      } else {
        printf("Stopped, but not at main. Current address: 0x%llx\n",
               (unsigned long long)regs.rip);
      }
    }

    // long orig_data = (data & 0xFFFFFFFFFFFFFF00) | (data & 0xFF);
    // if (ptrace(PTRACE_POKETEXT, pid, (void *)main_addr, (void *)orig_data) ==
    //     -1) {
    //   perror("ptrace(PTRACE_POKETEXT) to restore original instruction");
    //   return;
    // }

    // // Adjust RIP and continue
    // regs.rip = main_addr;
    // if (ptrace(PTRACE_SETREGS, pid, NULL, &regs) == -1) {
    //   perror("ptrace(PTRACE_SETREGS)");
    //   return;
    // }

    // Call the function that maps the address to a source line
    // if (get_line_from_pc(ui->dbg->dwarf, relative_pc, &filename,
    //                      &line_number) == DW_DLV_OK) {
    //   printf("Source file: %s\n", filename);
    //   printf("Line number: %llu\n", line_number);
    // } else {
    //   printf("No matching line found for PC: %llx\n",
    //          (unsigned long long)relative_pc);
    // }

    // step_view_t *view = &((ui_t
    // *)glfwGetWindowUserPointer(window))->step_view; view->curr_stepline =
    // (view->curr_stepline + 1) % view->source_lines.length;
  }
}

void
step_view_render(step_view_t *view) {
  {
    ImVec2 window_size;
    igGetWindowSize(&window_size);

    igBegin("test_files/funcs.c", NULL, 0);
    step_line_t_slice step_line = view->source_lines;
    for (size_t i = 0; i < step_line.length; i++) {
      const char *text = step_line_t_slice_index(&step_line, i).source_line;
      if (view->curr_stepline == i) {
        ImDrawList *draw_list = igGetWindowDrawList();

        ImVec2 text_pos, text_size;
        igGetCursorScreenPos(&text_pos);
        // printf("L: %s\n", step_line);
        igCalcTextSize(&text_size, text, NULL, false, -1.0f);
        ImVec2 rect_min = text_pos;
        ImVec2 rect_max = {text_pos.x + window_size.x,
                           text_pos.y + text_size.y};
        ImVec4 color    = {255, 0, 0, 255};
        ImDrawList_AddRectFilled(draw_list, rect_min, rect_max,
                                 igGetColorU32_Vec4(color), 0.0f, 0);
      }
      igText("%d: %s", step_line_t_slice_index(&step_line, i).source_num, text);
    }
    igEnd();
  }
}