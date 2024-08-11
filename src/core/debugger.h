#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdint.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include "breakpoint.h"
#include "slice.h"

SLICE_DEFINITION(breakpoint_t);

typedef struct {
  char*              prog_name;
  int                pid;
  breakpoint_t_slice breakpoints;
} debugger_t;

debugger_t
debugger(char* prog_name, int pid);

void
debugger_run(debugger_t* dbg);

void
debugger_free(debugger_t* dbg);

void
continue_execution(debugger_t* dbg);

uintptr_t
find_main_address(pid_t pid);

uintptr_t
get_base_address(pid_t pid);

long
read_memory(debugger_t* dbg, uint64_t addr);

long
write_memory(debugger_t* dbg, uint64_t addr, uint64_t value);

#endif  // DEBUGGER_H