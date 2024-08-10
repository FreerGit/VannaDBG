#include "breakpoint.h"

#include <stdint.h>
#include <sys/ptrace.h>

breakpoint_t
breakpoint(int pid, intptr_t addr) {
  return (breakpoint_t){
      .pid        = pid,
      .addr       = addr,
      .enabled    = 0,
      .saved_data = 0,
  };
}

void
breakpoint_enable(breakpoint_t* brkpt) {
  // long data         = ptrace(PTRACE_PEEKDATA, brkpt->pid, brkpt->addr,
  // nullptr); brkpt->saved_data = data & 0xff;  // bottom byte uint64_t int3 =
  // 0xcc; uint64_t data_with_int3 =
  //     ((data & ~0xff) | int3);  // set bottom byte to 0xcc;
  // ptrace(PTRACE_POKEDATA, brkpt->pid, brkpt->addr, data_with_int3);
  // brkpt->enabled = true;
}

void
breakpoint_disable(breakpoint_t* brkpt) {
  // auto data = ptrace(PTRACE_PEEKDATA, brkpt->pid, brkpt->addr, nullptr);
  // auto restored_data = ((data & ~0xff) | brkpt->saved_data);
  // ptrace(PTRACE_POKEDATA, brkpt->pid, brkpt->addr, restored_data);
  // brkpt->enabled = false;
}

bool
breakpoint_is_enabled(breakpoint_t* brkpt) {
  return brkpt->enabled;
}

intptr_t
breakpoint_get_address(breakpoint_t* brkpt) {
  return brkpt->addr;
}
