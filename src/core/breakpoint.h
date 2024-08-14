#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <stdint.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

typedef struct {
  intptr_t addr;
  int      pid;
  bool     enabled;
} breakpoint_t;

breakpoint_t
breakpoint(int pid, intptr_t addr);

breakpoint_t
breakpoint_create(int pid, uint32_t source_num);

void
breakpoint_disable(breakpoint_t* brkpt);

// bool
// breakpoint_is_enabled(breakpoint_t* brkpt);

intptr_t
breakpoint_get_address(breakpoint_t* brkpt);

#endif  // BREAKPOINT_H