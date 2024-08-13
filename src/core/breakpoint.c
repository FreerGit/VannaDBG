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
      .pid     = pid,
      .addr    = addr,
      .enabled = 0,
  };
}

// TODO this should take a line of source later on.
breakpoint_t
breakpoint_create(int pid, uint32_t source_num) {
  uintptr_t addr = find_address_by_line(
      "./test_files/a.out", "/home/a7/dev/VannaDBG/test_files/breakpoints.c",
      source_num);
  addr += get_base_address(pid);
  printf("base addr: 0x%lx\nfound addr: 0x%lx\n", get_base_address(pid), addr);

  // assert((memcmp(name, "main", 5) == 0) && "needs to be main");
  // const uintptr_t main_adddr = find_main_address(pid);

  long orig_data = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);
  if (orig_data == -1) {
    perror("ptrace PEEKTEXT");
    exit(EXIT_FAILURE);
  }

  long int3 =
      (orig_data & ~0xFF) | 0xCC;  // Replace the first byte with 0xCC (INT 3)

  assert(ptrace(PTRACE_POKETEXT, pid, addr, int3) != -1);

  return (breakpoint_t){pid, addr, true};
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
