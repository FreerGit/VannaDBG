

#include "debugger.h"

#include <gelf.h>
#include <libelf.h>
#include <stdint.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#include "breakpoint.h"
#include "dwarf/conversion.h"
#include "registers.h"

size_t
addr_hash(const void* key) {
  uintptr_t value = *(const uintptr_t*)key;
  value ^= (value >> 32);
  return (size_t)value;
}

static int comparisons = 0;

bool
addr_eql(const void* key1, const void* key2) {
  comparisons++;
  return *(const uintptr_t*)key1 == *(const uintptr_t*)key2;
}

debugger_t
debugger(arena_t* arena, char* prog_name, int pid) {
  uintptr_t base_addr = get_base_address(pid);
  hashmap_t map =
      hashmap_create(arena, 100, sizeof(breakpoint_t), addr_hash, addr_eql);
  return (debugger_t){
      .prog_name   = prog_name,
      .pid         = pid,
      .base_addr   = base_addr,
      .breakpoints = map,
  };
}

// char**
// split(char* str, char* delim, size_t* i) {
//   char** out   = malloc(256 * sizeof(char*));
//   char*  token = strtok(str, delim);
//   while (token != nullptr) {
//     out[(*i)++] = token;
//     token       = strtok(nullptr, delim);
//   }
//   return out;
// }

uint64_t
get_pc(debugger_t* dbg) {
  return get_register_value(dbg->pid, rip);
}

void
set_pc(debugger_t* dbg, uint64_t pc) {
  set_register_value(dbg->pid, rip, pc);
}

void
wait_for_signal(debugger_t* dbg) {
  int wait_status;
  int options = 0;
  waitpid(dbg->pid, &wait_status, options);
}

void
set_breakpoint_at_addr(debugger_t* dbg, intptr_t addr) {
  breakpoint_t bp = {.addr = addr + dbg->base_addr, .pid = dbg->pid};
  breakpoint_enable(&bp);
  hashmap_put(&dbg->breakpoints, &bp.addr, &bp, sizeof(intptr_t));
}

void
remove_breakpoint_at_addr(debugger_t* dbg, intptr_t addr) {
  const intptr_t absolute_addr = addr + dbg->base_addr;
  breakpoint_t*  bp            = hashmap_get(&dbg->breakpoints, &absolute_addr);
  assert(bp != nullptr);
  hashmap_remove(&dbg->breakpoints, &addr);
  breakpoint_disable(bp);
}

void
step_over_breakpoint(debugger_t* dbg) {
  // Execution will go past breakpoint.
  uint64_t      maybe_breakpoint = get_pc(dbg) - 1;
  breakpoint_t* bp = hashmap_get(&dbg->breakpoints, &maybe_breakpoint);
  printf("fouind\n");
  if (bp != NULL && bp->enabled) {
    printf("here\n");
    uint64_t prev_instruction_addr = maybe_breakpoint;
    set_pc(dbg, prev_instruction_addr);
    breakpoint_disable(bp);
    ptrace(PTRACE_SINGLESTEP, dbg->pid, nullptr, nullptr);
    wait_for_signal(dbg);
    breakpoint_enable(bp);
  }
}

void
continue_execution(debugger_t* dbg) {
  step_over_breakpoint(dbg);
  ptrace(PTRACE_CONT, dbg->pid, nullptr, nullptr);
  wait_for_signal(dbg);
}

long
read_memory(debugger_t* dbg, uint64_t addr) {
  return ptrace(PTRACE_PEEKDATA, dbg->pid, addr, nullptr);
}

long
write_memory(debugger_t* dbg, uint64_t addr, uint64_t value) {
  ptrace(PTRACE_POKEDATA, dbg->pid, addr, value);
}