#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <stdint.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

typedef struct {
  intptr_t addr;
  int      pid;
  uint8_t  saved_data;
  bool     enabled;
} breakpoint_t;

static_assert(sizeof(breakpoint_t) == 16);

breakpoint_t
breakpoint(int pid, intptr_t addr);

void
breakpoint_enable(breakpoint_t* bp);

void
breakpoint_disable(breakpoint_t* bp);

#endif  // BREAKPOINT_H