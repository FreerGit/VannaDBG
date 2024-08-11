

#include "debugger.h"

#include <errno.h>
#include <gelf.h>
#include <libelf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#include "breakpoint.h"
#include "registers.h"

typedef struct {
  intptr_t     addr;
  breakpoint_t bp;
} breakpoint_addr;

uintptr_t
find_main_address(pid_t pid) {
  char proc_exe[64];
  snprintf(proc_exe, sizeof(proc_exe), "/proc/%d/exe", pid);

  FILE* fp = fopen(proc_exe, "r");
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

  Elf* elf = elf_begin(fd, ELF_C_READ, NULL);
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

  Elf_Scn*  scn = NULL;
  GElf_Shdr shdr;
  Elf_Data* data;
  int       symbol_count;

  while ((scn = elf_nextscn(elf, scn)) != NULL) {
    gelf_getshdr(scn, &shdr);
    if (shdr.sh_type == SHT_SYMTAB) {
      data         = elf_getdata(scn, NULL);
      symbol_count = shdr.sh_size / shdr.sh_entsize;

      for (int i = 0; i < symbol_count; i++) {
        GElf_Sym sym;
        gelf_getsym(data, i, &sym);

        char* name = elf_strptr(elf, shdr.sh_link, sym.st_name);
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

  FILE* fp = fopen(filename, "r");
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

debugger_t
debugger(char* prog_name, int pid) {
  debugger_t dbg = (debugger_t){
      .prog_name   = prog_name,
      .pid         = pid,
      .breakpoints = {},
  };

  return dbg;
}

// char**
// split(char* str, char* delim, size_t* i) {
//   char** out   = malloc(256 * sizeof(char*));
//   char*  token = strtok(str, delim);
//   while (token != nullptr) {
//     out[(*i)++] = token;
//     token       = strtok(nullptr, delim);
//   }
//   return out;
// }

// void
// set_breakpoint_at_addr(debugger_t* dbg, intptr_t addr) {
//   printf("Set breakpoint at addr 0x%li (0x%ld)\n", addr, (uint64_t)addr);
//   breakpoint_t bp = {dbg->pid, addr, .enabled = false};
//   breakpoint_enable(&bp);
//   hashmap_set(dbg->breakpoints, &(breakpoint_addr){.addr = addr, .bp = bp});
// }

// bool
// str_eql(const char* s1, const char* s2) {
//   return strcmp(s1, s2) == 0;
// }

// uint64_t
// addr_to_value(const char* addr) {
//   char* endptr;
//   errno = 0;

//   uint64_t result = strtol(addr, &endptr, 16);

//   if (errno == ERANGE) {
//     assert(0 && "Conversion error: Number out of range\n");
//   } else if (str_eql(endptr, addr)) {
//     assert(0 && "Conversion error: No digits were found\n");
//   } else if (*endptr != '\0') {
//     assert(0 && "Conversion warning: Not all characters were converted\n");
//   }
//   return result;
// }

// void
// dump_registers(debugger_t* dbg) {
//   for (size_t i = 0; i < n_registers; i++) {
//     reg_description_t rd = reg_desc_array[i];
//     printf("%s 0x%016lx\n", rd.name, get_register_value(dbg->pid, rd.r));
//   }
// }

// You might want to add support for reading and writing more than a word at a
// time, which you can do by incrementing the address each time you want to read
// another word. You could also use process_vm_readv and process_vm_writev or
// /proc/<pid>/mem instead of ptrace if you like.

long
read_memory(debugger_t* dbg, uint64_t addr) {
  return ptrace(PTRACE_PEEKDATA, dbg->pid, addr, nullptr);
}

long
write_memory(debugger_t* dbg, uint64_t addr, uint64_t value) {
  ptrace(PTRACE_POKEDATA, dbg->pid, addr, value);
}

uint64_t
get_pc(debugger_t* dbg) {
  return get_register_value(dbg->pid, rip);
}

void
set_pc(debugger_t* dbg, uint64_t pc) {
  set_register_value(dbg->pid, rip, pc);
}

void
wait_for_signal(debugger_t* dbg) {
  int wait_status;
  int options = 0;
  waitpid(dbg->pid, &wait_status, options);
}

// void
// step_over_breakpoint(debugger_t* dbg) {
//   uint64_t breakpoint_location = get_pc(dbg) - 1;
//   printf("loc: %li at ptr: %p\n", (intptr_t)breakpoint_location,
//          &breakpoint_location);

//   breakpoint_addr* bpaddr = (breakpoint_addr*)hashmap_get(
//       dbg->breakpoints, &(breakpoint_addr){.addr = breakpoint_location});
//   printf("bpaddr: %p\n", bpaddr);
//   if (bpaddr != NULL) {
//     if (breakpoint_is_enabled(&bpaddr->bp)) {
//       uint64_t previous_instr = breakpoint_location;
//       set_pc(dbg, previous_instr);
//       breakpoint_disable(&bpaddr->bp);
//       ptrace(PTRACE_SINGLESTEP, dbg->pid, nullptr, nullptr);
//       wait_for_signal(dbg);
//       breakpoint_enable(&bpaddr->bp);
//     }
//   }
// }

// void
// continue_execution(debugger_t* dbg) {
//   step_over_breakpoint(dbg);
//   ptrace(PTRACE_CONT, dbg->pid, nullptr, nullptr);
//   wait_for_signal(dbg);
// }

// void
// handle_command(debugger_t* dbg, char* line) {
//   size_t size    = 0;
//   char** args    = split(line, " ", &size);
//   char*  command = args[0];

//   if (str_eql(command, "cont")) {
//     continue_execution(dbg);
//   } else if (str_eql(command, "break")) {
//     // TODO
//     char* str_addr = args[1] + 2;  // Assume 0xADDR....
//     set_breakpoint_at_addr(dbg, addr_to_value(str_addr));
//   } else if (str_eql(command, "register")) {
//     if (str_eql(args[1], "dump")) {
//       // dump_registers(dbg);
//     } else if (str_eql(args[1], "read")) {
//       printf("%ld\n",
//              get_register_value(dbg->pid, get_register_from_name(args[2])));
//     } else if (str_eql(args[1], "write")) {
//       char* str_addr = args[3] + 2;  // Assume 0xADDR...
//       set_register_value(dbg->pid, get_register_from_name(args[2]),
//                          addr_to_value(str_addr));
//     } else if (str_eql(command, "memory")) {
//       if (str_eql(args[1], "read")) {
//         char* str_addr = args[2] + 2;  // Assume 0xADDR...
//         printf("0x%lu\n", read_memory(dbg, addr_to_value(str_addr)));
//       } else if (str_eql(command, "write")) {
//         char* str_addr = args[3] + 2;  // Assume 0xVAL...
//         write_memory(dbg, addr_to_value(str_addr), addr_to_value(str_addr));
//       }
//     }
//   } else {
//     fprintf(stderr, "Unknown command: %s\n", command);
//   }

//   free(args);
// }

// void
// debugger_run(debugger_t* dbg) {
//   wait_for_signal(dbg);
//   // char* line = NULL;
//   // while ((line = linenoise("VannaDBG> ")) != nullptr) {
//   //   handle_command(dbg, line);
//   //   linenoiseHistoryAdd(line);
//   //   linenoiseFree(line);
//   // }
// }

// void
// debugger_free(debugger_t* dbg) {
//   hashmap_free(dbg->breakpoints);
// }
