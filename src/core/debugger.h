#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdint.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include "domain/hashmap.h"

typedef struct {
  arena_t   arena;
  hashmap_t breakpoints;
  uintptr_t base_addr;
  char*     prog_name;
  int       pid;
} debugger_t;

static_assert(sizeof(debugger_t) == 96);

debugger_t
debugger(arena_t* arean, char* prog_name, int pid);

void
debugger_run(debugger_t* dbg);

void
debugger_free(debugger_t* dbg);

void
continue_execution(debugger_t* dbg);

void
set_breakpoint_at_addr(debugger_t* dbg, intptr_t addr);

void
remove_breakpoint_at_addr(debugger_t* dbg, intptr_t addr);

void
step_over_breakpoint(debugger_t* dbg);

long
read_memory(debugger_t* dbg, uint64_t addr);

long
write_memory(debugger_t* dbg, uint64_t addr, uint64_t value);

uint64_t
get_pc(debugger_t* dbg);

#endif  // DEBUGGER_H