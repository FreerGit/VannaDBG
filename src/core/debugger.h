#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdint.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include "hashmap.h"

typedef struct {
  char*           prog_name;
  int             pid;
  struct hashmap* breakpoints;
} debugger_t;

debugger_t
debugger(char* prog_name, int pid);

void
debugger_run(debugger_t* dbg);

void
debugger_free(debugger_t* dbg);

void
continue_execution(debugger_t* dbg);

#endif  // DEBUGGER_H