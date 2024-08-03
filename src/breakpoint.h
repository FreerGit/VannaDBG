#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <stdint.h>
#include <sys/ptrace.h>

typedef struct {
  int     pid;
  char*   addr;
  bool    enabled;
  uint8_t saved_data;
} breakpoint_t;

breakpoint_t
breakpoint(int pid, char* addr);

void
breakpoint_enable(breakpoint_t* brkpt);

void
breakpoint_disable(breakpoint_t* brkpt);

bool
breakpoint_is_enabled(breakpoint_t* brkpt);

char*
breakpoint_get_address(breakpoint_t* brkpt);

#endif  // BREAKPOINT_H