

#include <dwarf.h>
#include <libdwarf.h>
#include <stdio.h>
#include <sys/personality.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#include "core/debugger.h"
#include "ui/ui.h"

void
execute_debugee(char *prog_name) {
  if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
    printf("Error in ptrace\n");
    return;
  }
  execl(prog_name, prog_name, NULL);
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
    debugger_run(&dbg);
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