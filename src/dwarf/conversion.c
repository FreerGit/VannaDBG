#include "conversion.h"

#include <assert.h>
#include <elf.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "domain/timing.h"
#include "elfutils/libdw.h"
#include "elfutils/libdwfl.h"

uintptr_t
find_main_address(pid_t pid) {
  char proc_exe[64];
  snprintf(proc_exe, sizeof(proc_exe), "/proc/%d/exe", pid);

  FILE *fp = fopen(proc_exe, "r");
  int   fd = fileno(fp);
  if (fd < 0) {
    perror("Failed to open executable");
    return 0;
  }

  if (elf_version(EV_CURRENT) == EV_NONE) {
    fprintf(stderr, "ELF library initialization failed\n");
    close(fd);
    return 0;
  }

  Elf *elf = elf_begin(fd, ELF_C_READ, NULL);
  if (!elf) {
    fprintf(stderr, "elf_begin() failed\n");
    close(fd);
    return 0;
  }

  GElf_Ehdr ehdr;
  if (gelf_getehdr(elf, &ehdr) == NULL) {
    fprintf(stderr, "gelf_getehdr() failed\n");
    elf_end(elf);
    close(fd);
    return 0;
  }

  Elf_Scn  *scn = NULL;
  GElf_Shdr shdr;
  Elf_Data *data;
  int       symbol_count;

  while ((scn = elf_nextscn(elf, scn)) != NULL) {
    gelf_getshdr(scn, &shdr);
    if (shdr.sh_type == SHT_SYMTAB) {
      data         = elf_getdata(scn, NULL);
      symbol_count = shdr.sh_size / shdr.sh_entsize;

      for (int i = 0; i < symbol_count; i++) {
        GElf_Sym sym;
        gelf_getsym(data, i, &sym);

        char *name = elf_strptr(elf, shdr.sh_link, sym.st_name);
        if (name && strcmp(name, "main") == 0) {
          elf_end(elf);
          close(fd);
          return (uintptr_t)sym.st_value + get_base_address(pid);
        }
      }
    }
  }

  elf_end(elf);
  close(fd);
  return 0;
}

uintptr_t
get_base_address(pid_t pid) {
  char filename[64];
  snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);

  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("Failed to open maps file");
    return 0;
  }

  char      line[256];
  uintptr_t base_addr = 0;

  while (fgets(line, sizeof(line), fp)) {
    unsigned long start, end;
    // Parse the line
    int matched = sscanf(line, "%lx-%lx", &start, &end);

    // Look for the executable segment
    if (matched == 2) {
      base_addr = start;
      break;
    }
  }

  fclose(fp);
  return base_addr;
}

// Function to find the address corresponding to a line in a source file
uintptr_t
find_address_by_line(const char *exe_path, const char *source_path,
                     int line_number) {
  Dwfl_Callbacks callbacks = {.find_elf        = dwfl_linux_proc_find_elf,
                              .find_debuginfo  = dwfl_standard_find_debuginfo,
                              .section_address = dwfl_offline_section_address};

  Dwfl *dwfl = dwfl_begin(&callbacks);
  if (!dwfl) {
    fprintf(stderr, "dwfl_begin: %s\n", dwfl_errmsg(-1));
    return 0;
  }

  Dwfl_Module *module = dwfl_report_offline(dwfl, exe_path, exe_path, -1);
  if (!module) {
    fprintf(stderr, "dwfl_report_offline: %s\n", dwfl_errmsg(-1));
    dwfl_end(dwfl);
    return 0;
  }

  if (dwfl_report_end(dwfl, NULL, NULL) != 0) {
    fprintf(stderr, "dwfl_report_end: %s\n", dwfl_errmsg(-1));
    dwfl_end(dwfl);
    return 0;
  }

  // Iterate over compilation units (CUs)
  Dwarf_Addr bias;
  Dwarf_Die *cu = NULL;
  while ((cu = dwfl_nextcu(dwfl, cu, &bias)) != NULL) {
    Dwarf_Lines *lines;
    size_t       nlines;

    // Get the lines associated with the CU
    if (dwarf_getsrclines(cu, &lines, &nlines) != 0) {
      fprintf(stderr, "dwarf_getsrclines: %s\n", dwarf_errmsg(-1));
      continue;
    }

    for (size_t i = 0; i < nlines; ++i) {
      Dwarf_Line *line = dwarf_onesrcline(lines, i);

      const char *src_file = dwarf_linesrc(line, NULL, NULL);
      if (!src_file) {
        continue;
      }

      int lineno;
      if (dwarf_lineno(line, &lineno) != 0) {
        fprintf(stderr, "dwarf_lineno: %s\n", dwarf_errmsg(-1));
        continue;
      }

      Dwarf_Addr addr;
      if (dwarf_lineaddr(line, &addr) != 0) {
        fprintf(stderr, "dwarf_lineaddr: %s\n", dwarf_errmsg(-1));
        continue;
      }
      if (strcmp(src_file, source_path) == 0 && lineno == line_number) {
        dwfl_end(dwfl);
        printf("bias: 0x%lx\n", bias);
        return addr;
        // + bias;
      }
    }
  }

  dwfl_end(dwfl);
  return 0;  // Return 0 if the address was not found
}

int
get_source_line_from_rip(pid_t pid, uintptr_t rip) {
  static const Dwfl_Callbacks callbacks = {
      .find_elf        = dwfl_linux_proc_find_elf,
      .find_debuginfo  = dwfl_standard_find_debuginfo,
      .section_address = dwfl_linux_kernel_module_section_address,
  };
  Dwfl              *dwfl;
  unsigned long long ns = TIME_A_BLOCK_NS({
    // Initialize DWFL session
    dwfl = dwfl_begin(&callbacks);
    assert(dwfl != NULL);

    // Report the process's modules
    if (dwfl_linux_proc_report(dwfl, pid) == -1) {
      fprintf(stderr, "Failed to report modules for process %d: %s\n", pid,
              dwfl_errmsg(-1));
      dwfl_end(dwfl);
      return -1;
    }

    // Complete the module reporting
    if (dwfl_report_end(dwfl, NULL, NULL) == -1) {
      fprintf(stderr, "Failed to complete module reporting.\n");
      dwfl_end(dwfl);
      return -1;
    }
  });

  printf("start micros: %lld\n", ns / 1000);
  Dwfl_Module *module;
  ns = TIME_A_BLOCK_NS({
    // Find the module associated with the given RIP
    module = dwfl_addrmodule(dwfl, rip);
    if (!module) {
      fprintf(stderr, "Failed to find module for address: %lx\n", rip);
      dwfl_end(dwfl);
      return -1;
    }
  });

  printf("addrmodule micros: %lld\n", ns / 1000);

  ns = TIME_A_BLOCK_NS({
    // Get the source file and line number from the address
    const char *filename;
    int         line;
    Dwfl_Line  *dwfl_line = dwfl_module_getsrc(module, rip);
    if (dwfl_line) {
      // filename = dwfl_lineinfo(dwfl_line, &rip, &line, NULL, NULL, NULL);
      // if (filename) {
      // printf("Instruction pointer %lx is in file %s at line %d\n", rip,
      //        filename, line);
      dwfl_end(dwfl);
      return line;
      // }
    }
  });
  printf("getsrc micros: %lld\n", ns / 1000);

  dwfl_end(dwfl);
  // fprintf(stderr, "No source information found for address: %lx\n", rip);
  return -1;
}
// int step_over_source_line(Dwfl *dwfl, uintptr_t *rip) {
//     Dwfl_Module *module = dwfl_addrmodule(dwfl, *rip);
//     if (!module) {
//         fprintf(stderr, "Failed to find module for address: %lx\n", *rip);
//         return -1;
//     }

//     const char *filename;
//     int line;
//     Dwfl_Line *current_line = dwfl_module_getsrc(module, *rip);
//     if (!current_line) {
//         fprintf(stderr, "No source information found for address: %lx\n",
//         *rip); return -1;
//     }

//     filename = dwfl_lineinfo(current_line, rip, &line, NULL, NULL, NULL);
//     if (!filename) {
//         fprintf(stderr, "Failed to get line info for address: %lx\n", *rip);
//         return -1;
//     }

//     while (1) {
//         *rip += 1; // Adjust this value as necessary depending on instruction
//         size and architecture Dwfl_Line *next_line =
//         dwfl_module_getsrc(module, *rip); if (!next_line) {
//             fprintf(stderr, "Reached the end of the line or function.\n");
//             return -1;
//         }

//         const char *next_filename = dwfl_lineinfo(next_line, rip, &line,
//         NULL, NULL, NULL); if (next_filename && (next_line != current_line))
//         {
//             printf("Stepped to new source line %d in file %s\n", line,
//             next_filename); break;
//         }
//     }

//     return 0;
// }

// int main(int argc, char **argv) {
//     Dwfl *dwfl = dwfl_begin(&callbacks);
//     if (!dwfl) {
//         fprintf(stderr, "Failed to initialize Dwfl session.\n");
//         return EXIT_FAILURE;
//     }

//     dwfl_report_begin(dwfl);

//     uintptr_t rip = 0x400123; // Replace with the actual RIP address
//     int ret = step_over_source_line(dwfl, &rip);

//     dwfl_end(dwfl);
//     return ret;
// }

// caching

// // Structure to store cached line information
// typedef struct {
//     uintptr_t start_addr;
//     uintptr_t end_addr;
//     const char *filename;
//     int line;
// } SourceLineInfo;

// // Example of storing all modules' line information
// typedef struct {
//     Dwfl_Module *module;
//     SourceLineInfo *lines;
//     size_t line_count;
// } ModuleCache;

// ModuleCache *cache = NULL;
// size_t cache_size = 0;

// int cache_module_info(Dwfl *dwfl, Dwfl_Module *module) {
//     // Get module's address range
//     GElf_Addr start, end;
//     dwfl_module_info(module, NULL, &start, &end, NULL, NULL, NULL, NULL);

//     // Retrieve source lines and cache them
//     size_t line_count = 0;
//     Dwfl_Line *dwfl_line = NULL;
//     while ((dwfl_line = dwfl_module_getsrc(module, start)) != NULL) {
//         GElf_Addr addr;
//         const char *filename = dwfl_lineinfo(dwfl_line, &addr, NULL, NULL,
//         NULL, NULL); if (filename) {
//             cache[cache_size].lines = realloc(cache[cache_size].lines,
//             sizeof(SourceLineInfo) * (line_count + 1));
//             cache[cache_size].lines[line_count].start_addr = addr;
//             cache[cache_size].lines[line_count].end_addr = addr + 1; // You
//             may want to expand this range
//             cache[cache_size].lines[line_count].filename = filename;
//             cache[cache_size].lines[line_count].line = -1; // Store line info
//             if needed line_count++;
//         }
//         start = addr + 1;
//     }

//     cache[cache_size].module = module;
//     cache[cache_size].line_count = line_count;
//     cache_size++;

//     return 0;
// }

// int build_cache(pid_t pid) {
//     static const Dwfl_Callbacks callbacks = {
//         .find_elf        = dwfl_linux_proc_find_elf,
//         .find_debuginfo  = dwfl_standard_find_debuginfo,
//         .section_address = dwfl_linux_kernel_module_section_address,
//     };

//     Dwfl *dwfl = dwfl_begin(&callbacks);
//     assert(dwfl != NULL);

//     if (dwfl_linux_proc_report(dwfl, pid) == -1) {
//         fprintf(stderr, "Failed to report modules for process %d: %s\n", pid,
//         dwfl_errmsg(-1)); dwfl_end(dwfl); return -1;
//     }

//     if (dwfl_report_end(dwfl, NULL, NULL) == -1) {
//         fprintf(stderr, "Failed to complete module reporting.\n");
//         dwfl_end(dwfl);
//         return -1;
//     }

//     Dwfl_Module *module = NULL;
//     while ((module = dwfl_nextmodule(dwfl, module)) != NULL) {
//         cache_module_info(dwfl, module);
//     }

//     dwfl_end(dwfl);
//     return 0;
// }

// void free_cache() {
//     for (size_t i = 0; i < cache_size; i++) {
//         free(cache[i].lines);
//     }
//     free(cache);
// }

// const SourceLineInfo *lookup_line_from_cache(uintptr_t rip) {
//     for (size_t i = 0; i < cache_size; i++) {
//         for (size_t j = 0; j < cache[i].line_count; j++) {
//             if (rip >= cache[i].lines[j].start_addr && rip <=
//             cache[i].lines[j].end_addr) {
//                 return &cache[i].lines[j];
//             }
//         }
//     }
//     return NULL;
// }