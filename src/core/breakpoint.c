#include "breakpoint.h"

#include <assert.h>
#include <dwarf/conversion.h>
#include <elf.h>
#include <elfutils/libdw.h>
#include <gelf.h>
#include <libelf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

breakpoint_t
breakpoint(int pid, intptr_t addr) {
  return (breakpoint_t){
      .pid = pid, .addr = addr, .enabled = 0, .saved_data = 0};
}

void
breakpoint_enable(breakpoint_t* bp) {
  printf("pid: %d, addr: %lx\n", bp->pid, bp->addr);
  long data = ptrace(PTRACE_PEEKDATA, bp->pid, bp->addr, NULL);
  assert(data != -1 && "PEEKDATA");
  // if (data == -1) {
  // perror("ptrace PEEKTEXT");
  // exit(EXIT_FAILURE);
  // }

  bp->saved_data = data & 0xFF;
  long int3 =
      (data & ~0xFF) | 0xCC;  // Replace the first byte with 0xCC (INT 3)
  assert(ptrace(PTRACE_POKETEXT, bp->pid, bp->addr, int3) != -1);
  bp->enabled = true;
}

void
breakpoint_disable(breakpoint_t* brkpt) {
  auto data = ptrace(PTRACE_PEEKDATA, brkpt->pid, brkpt->addr, nullptr);
  assert(data != -1);
  auto restored_data = ((data & ~0xff) | brkpt->saved_data);
  ptrace(PTRACE_POKEDATA, brkpt->pid, brkpt->addr, restored_data);
  brkpt->enabled = false;
}
