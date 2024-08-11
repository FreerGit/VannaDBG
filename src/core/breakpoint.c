#include "breakpoint.h"

#include <stdint.h>
#include <sys/ptrace.h>

breakpoint_t
breakpoint(int pid, intptr_t addr) {
  return (breakpoint_t){
      .pid = pid, .addr = addr,
      // .enabled    = 0,
      // .saved_data = 0,
  };
}

// TODO this should take a line of source later on.
breakpoint_t
breakpoint_create(int pid, char* name) {
}

void
breakpoint_disable(breakpoint_t* brkpt) {
  // auto data = ptrace(PTRACE_PEEKDATA, brkpt->pid, brkpt->addr, nullptr);
  // auto restored_data = ((data & ~0xff) | brkpt->saved_data);
  // ptrace(PTRACE_POKEDATA, brkpt->pid, brkpt->addr, restored_data);
  // brkpt->enabled = false;
}

intptr_t
breakpoint_get_address(breakpoint_t* brkpt) {
  return brkpt->addr;
}
