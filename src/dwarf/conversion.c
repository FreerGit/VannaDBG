
#include "conversion.h"

#include <elf.h>
#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
      printf("linno: %d\nsrc file: %s\n", lineno, src_file);
      // Check if the line matches the requested file and line number
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