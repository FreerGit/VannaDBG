#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <sys/ptrace.h>
#include <sys/wait.h>

#include "../ext/hashmap/hashmap.h"

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

#endif  // DEBUGGER_H