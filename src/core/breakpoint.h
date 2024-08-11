#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <stdint.h>
#include <sys/ptrace.h>

typedef struct {
  int      pid;
  intptr_t addr;
  bool     x;
} breakpoint_t;

breakpoint_t
breakpoint(int pid, intptr_t addr);

breakpoint_t
breakpoint_create(int pid, char* name);

void
breakpoint_disable(breakpoint_t* brkpt);

// bool
// breakpoint_is_enabled(breakpoint_t* brkpt);

intptr_t
breakpoint_get_address(breakpoint_t* brkpt);

#endif  // BREAKPOINT_H