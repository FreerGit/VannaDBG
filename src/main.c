

// #include <dwarf.h>
#include <libdwarf.h>
// #include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/personality.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include "core/debugger.h"
#include "dwarf.h"
#include "ui/ui.h"

void
execute_debugee(char *prog_name) {
  if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
    printf("Error in ptrace\n");
    return;
  }

  execl(prog_name, prog_name, NULL);
  perror("execl");
  exit(1);
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
initialize_dwarf(const char *filename, Dwarf_Debug *dwarf_out) {
  Dwarf_Debug dwarf;
  Dwarf_Error error = NULL;
  // int         fd;
  int res;

  // Open the file
  FILE *fp = fopen(filename, "r");
  int   fd = fileno(fp);
  if (fd < 0) {
    perror("Failed to open file");
    return DW_DLV_ERROR;
  }

  // Initialize DWARF debugging information
  res = dwarf_init_b(fd, DW_GROUPNUMBER_ANY, NULL, NULL, &dwarf, &error);
  if (res != DW_DLV_OK) {
    close(fd);
    check_error(error, "Failed to initialize DWARF debugging information");
    return res;
  }

  *dwarf_out = dwarf;
  return DW_DLV_OK;
}

Dwarf_Addr
find_main_address(Dwarf_Debug dbg, Dwarf_Addr base_addr) {
  Dwarf_Error error  = NULL;
  Dwarf_Die   no_die = NULL, cu_die, child_die;
  Dwarf_Addr  low_pc;
  Dwarf_Half  tag;
  char       *name;
  Dwarf_Bool  is_info = 1;  // Assume we're working with .debug_info

  while (dwarf_next_cu_header_d(dbg, is_info, NULL, NULL, NULL, NULL, NULL,
                                NULL, NULL, NULL, NULL, NULL,
                                &error) == DW_DLV_OK) {
    if (dwarf_siblingof_b(dbg, no_die, is_info, &cu_die, &error) == DW_DLV_OK) {
      if (dwarf_child(cu_die, &child_die, &error) == DW_DLV_OK) {
        while (1) {
          if (dwarf_tag(child_die, &tag, &error) == DW_DLV_OK) {
            if (tag == DW_TAG_subprogram) {
              if (dwarf_diename(child_die, &name, &error) == DW_DLV_OK) {
                if (strcmp(name, "main") == 0) {
                  if (dwarf_lowpc(child_die, &low_pc, &error) == DW_DLV_OK) {
                    // dwarf_dealloc_string(dbg, name);
                    printf("low pc: %llx\n", low_pc);
                    return low_pc + base_addr;
                  }
                }
                // dwarf_dealloc_string(dbg, name);
              }
            }
          }
          Dwarf_Die sibling_die;
          int       res =
              dwarf_siblingof_b(dbg, child_die, is_info, &sibling_die, &error);
          if (res == DW_DLV_OK) {
            dwarf_dealloc(dbg, child_die, DW_DLA_DIE);
            child_die = sibling_die;
          } else if (res == DW_DLV_NO_ENTRY) {
            break;
          } else {
            return 0;  // Error
          }
        }
      }
      // dwarf_dealloc(*dbg, cu_die, DW_DLA_DIE);
    }
  }
  return 0;  // main not found
}

int
set_breakpoint_at_main(pid_t pid, Dwarf_Debug dbg, Dwarf_Addr base_addr) {
  Dwarf_Addr main_addr = 0x555555555189;
  // find_main_address(dbg, bse_addr);
  // 0x555555555195;
  if (main_addr == 0) {
    fprintf(stderr, "Failed to find main function\n");
    return -1;
  }

  // Read the original instruction
  long original_data = ptrace(PTRACE_PEEKDATA, pid, (void *)main_addr, NULL);
  if (original_data == -1) {
    perror("ptrace(PTRACE_PEEKDATA)");
    return -1;
  }

  // Replace the first byte with the breakpoint instruction (INT3)
  long breakpoint_data = (original_data & ~0xFF) | 0xCC;

  // Write the breakpoint instruction
  if (ptrace(PTRACE_POKEDATA, pid, (void *)main_addr,
             (void *)breakpoint_data) == -1) {
    perror("ptrace(PTRACE_POKEDATA)");
    return -1;
  }

  printf("Breakpoint set at main: 0x%llx\n", main_addr);
  return 0;
}
// 0x555555556189
// 0x555555555195
Dwarf_Addr
get_base_address(pid_t pid) {
  // char filename[64];
  // snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);

  // FILE *fp = fopen(filename, "r");
  // if (fp == NULL) {
  //   perror("Failed to open maps file");
  //   return 0;
  // }

  // char       line[256];
  // Dwarf_Addr base_addr = 0;

  // while (fgets(line, sizeof(line), fp)) {
  //   unsigned long long start, end;
  //   char               permissions[5];
  //   char               path[256];

  //   // Parse the line
  //   int matched = sscanf(line, "%llx-%llx %4s %*x %*x:%*x %*d %255s", &start,
  //                        &end, permissions, path);

  //   // We're looking for the executable segment of our main program
  //   if (matched == 4 && strstr(permissions, "x") && strstr(path, "a.out")) {
  //     base_addr = start;
  //     break;
  //   }
  // }

  // fclose(fp);

  Dwarf_Addr base_addr = 0x555555554000;
  return base_addr;
}

int
main() {
  int pid = fork();
  if (pid == 0) {
    // TODO(improvement): ASLR is enabled for now, this should be user definable
    personality(ADDR_NO_RANDOMIZE);
    execute_debugee("./test_files/a.out");
  } else if (pid >= 1) {
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
      printf("Child exited\n");
      return -1;
    }
    if (!WIFSTOPPED(status)) {
      printf("Unexpected status\n");
      return -1;
    }

    printf("Started debugging process %d\n", pid);

    FILE *fp = fopen("./test_files/a.out", "r");
    int   fd = fileno(fp);
    assert(fp != NULL);
    assert(fd != 0);

    debugger_t dbgr = debugger("./test_files/a.out", pid);

    int res = initialize_dwarf("./test_files/a.out", &dbgr.dwarf);
    if (res != DW_DLV_OK) {
      fprintf(stderr, "Failed to initialize DWARF\n");
      return 1;
    }

    Dwarf_Addr base_addr = get_base_address(pid);
    printf("base addr: %llx\n", base_addr);

    if (set_breakpoint_at_main(pid, dbgr.dwarf, base_addr) == 0) {
      printf("Breakpoint set successfully\n");
    } else {
      fprintf(stderr, "Failed to set breakpoint\n");
    }

    // if (ptrace(PTRACE_CONT, pid, NULL, NULL) == -1) {
    //   perror("ptrace(PTRACE_CONT)");
    //   return;
    // }

    // waitpid(pid, &status, 0);

    // if (ptrace(PTRACE_CONT, pid, NULL, NULL) == -1) {
    //   perror("ptrace(PTRACE_CONT)");
    //   return;
    // }
    // waitpid(pid, &status, 0);

    // assert(false && "exit");
    // return 1;

    // debugger_run(&dbg);
    // debugger_free(&dbg);

    res = start_ui(&dbgr);
    // return res;

    // Dwarf_Debug dbg;
    // Dwarf_Error error = NULL;
    // // int         res;

    // res = dwarf_init_path("./test_files/a.out", NULL, 0, DW_GROUPNUMBER_ANY,
    //                       NULL, NULL, &dbg, &error);
    // if (res != DW_DLV_OK) {
    //   check_error(error, "Failed to initialize DWARF debugging information");
    // }

    // Dwarf_Unsigned     cu_header_length, abbrev_offset, next_cu_header;
    // Dwarf_Half         version_stamp, address_size;
    // Dwarf_Die          no_die       = 0, cu_die;
    // Dwarf_Unsigned     cu_number    = 0;
    // Dwarf_Line_Context line_context = 0;
    // Dwarf_Small        table_count  = 0;
    // Dwarf_Unsigned     lineversion  = 0;
    // // Dwarf_Error *error

    // while ((res = dwarf_next_cu_header_d(
    //             dbg, 1, &cu_header_length, &version_stamp, &abbrev_offset,
    //             &address_size, NULL, NULL, NULL, NULL, &next_cu_header, NULL,
    //             &error)) == DW_DLV_OK) {
    //   cu_number++;
    //   printf("\n--- Compilation Unit %llu ---\n", cu_number);
    //   printf("CU Header Length: %llu\n", cu_header_length);
    //   printf("Version Stamp: %hu\n", version_stamp);
    //   printf("Abbrev Offset: %llu\n", abbrev_offset);
    //   printf("Address Size: %hu\n", address_size);

    //   if (dwarf_siblingof_b(dbg, no_die, 1, &cu_die, &error) == DW_DLV_OK) {
    //     print_cu_name(dbg, cu_die);

    //     // Print CU offset
    //     Dwarf_Off cu_offset;
    //     if (dwarf_dieoffset(cu_die, &cu_offset, &error) == DW_DLV_OK) {
    //       printf("CU Offset: %llu\n", cu_offset);
    //     }

    //     // Print producer (compiler info)
    //     char *producer;
    //     if (dwarf_attr(cu_die, DW_AT_producer, &producer, &error) ==
    //         DW_DLV_OK) {
    //       printf("Producer: %s\n", producer);
    //       dwarf_dealloc(dbg, producer, DW_DLA_STRING);
    //     }

    //     // Print language
    //     Dwarf_Half language;
    //     if (dwarf_srclang(cu_die, &language, &error) == DW_DLV_OK) {
    //       printf("Language: %hu\n", language);
    //     }

    //     // Print line number info
    //     Dwarf_Line  *linebuf   = NULL;
    //     Dwarf_Signed linecount = 0;
    //     int          sres      = 0;
    //     if (dwarf_srclines_b(cu_die, &lineversion, &table_count,
    //     &line_context,
    //                          &error) == DW_DLV_OK) {
    //       printf("Number of lines: %hhu\n", table_count);
    //       Dwarf_Signed i          = 0;
    //       Dwarf_Signed baseindex  = 0;
    //       Dwarf_Signed file_count = 0;
    //       Dwarf_Signed endindex   = 0;
    //       sres = dwarf_srclines_files_indexes(line_context, &baseindex,
    //                                           &file_count, &endindex, error);
    //       if (sres != DW_DLV_OK) {
    //         /* Something badly wrong! */
    //         return sres;
    //       }

    //       sres = dwarf_srclines_from_linecontext(line_context, &linebuf,
    //                                              &linecount, error);
    //       if (sres != DW_DLV_OK) {
    //         /* Error. Clean up the context information. */
    //         dwarf_srclines_dealloc_b(line_context);
    //         return sres;
    //       }
    //       /* The lines are normal line table lines. */
    //       for (i = 0; i < linecount; ++i) {
    //         /* use linebuf[i] */
    //         Dwarf_Unsigned column;
    //         if (dwarf_lineno(linebuf[i], &column, &error) == DW_DLV_OK) {
    //           printf("Column: %llu\n", (unsigned long long)column);
    //         }
    //         // printf("%d", );
    //       }
    //       // print_line_info(dbg, cu_die);
    //       dwarf_dealloc(dbg, linebuf, DW_DLA_LIST);
    //     }

    //     dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
    //   }
    // }

    // if (res == DW_DLV_ERROR) {
    //   check_error(error, "Error in dwarf_next_cu_header_d");
    // }

    // // int res = 0;
    // return res;
  }
}