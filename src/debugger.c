
#include "debugger.h"

#include <linenoise.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include "breakpoint.h"
#include "hashmap.h"

typedef struct {
  char*        addr;
  breakpoint_t bp;
} breakpoint_addr;

int
breakpoint_addr_compare(const void* a, const void* b, void* udata) {
  const breakpoint_addr* aptr = a;
  const breakpoint_addr* bptr = b;
  return strcmp(aptr->addr, bptr->addr);
}

uint64_t
breakpoint_addr_hash(const void* item, uint64_t seed0, uint64_t seed1) {
  const breakpoint_addr* bpaddr = item;
  printf("hashing: %s\n", bpaddr->addr);
  return hashmap_sip((void*)bpaddr->addr, strlen(bpaddr->addr), seed0, seed1);
}

debugger_t
debugger(char* prog_name, int pid) {
  struct hashmap* map =
      hashmap_new(sizeof(breakpoint_addr), 1024, 0, 0, breakpoint_addr_hash,
                  breakpoint_addr_compare, NULL, NULL);
  return (debugger_t){.prog_name = prog_name, .pid = pid, .breakpoints = map};
}

void
continue_execution(debugger_t* dbg) {
  ptrace(PTRACE_CONT, dbg->pid, nullptr, nullptr);
  int wait_status;
  int options = 0;
  waitpid(dbg->pid, &wait_status, options);
}

char**
split(char* str, char* delim, size_t* i) {
  char** out   = malloc(256 * sizeof(char*));
  char*  token = strtok(str, delim);
  while (token != nullptr) {
    out[(*i)++] = token;
    token       = strtok(nullptr, delim);
  }
  return out;
}

void
set_breakpoint_at_addr(debugger_t* dbg, char* addr) {
  printf("Set breakpoint at addr 0x%s\n", addr);
  breakpoint_t bp = {dbg->pid, addr};
  breakpoint_enable(&bp);
  hashmap_set(dbg->breakpoints, &(breakpoint_addr){.addr = addr, .bp = bp});
}

void
handle_command(debugger_t* dbg, char* line) {
  size_t size    = 0;
  char** args    = split(line, " ", &size);
  char*  command = args[0];

  if (strcmp(command, "cont") == 0) {
    continue_execution(dbg);
  } else if (strcmp(command, "break") == 0) {
    // TODO
    char* str_addr = args[1];
    str_addr += 2;  // Assume 0xADDR....
    char* endptr;

    set_breakpoint_at_addr(dbg, str_addr);
  }

  else {
    fprintf(stderr, "Unknown command: %s\n", command);
  }

  free(args);
}

void
debugger_run(debugger_t* dbg) {
  int wait_status;
  int options = 0;

  char* line = NULL;
  while ((line = linenoise("VannaDBG> ")) != nullptr) {
    handle_command(dbg, line);
    linenoiseHistoryAdd(line);
    linenoiseFree(line);
  }
}

void
debugger_free(debugger_t* dbg) {
  hashmap_free(dbg->breakpoints);
}