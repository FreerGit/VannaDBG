#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/personality.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "debugger.h"

void
execute_debugee(char* prog_name) {
  if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
    printf("Error in ptrace\n");
    return;
  }
  execl(prog_name, prog_name, NULL);
}

int
main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Program name not specified\n");
    return -1;
  }

  char* prog = argv[1];

  int pid = fork();
  if (pid == 0) {
    personality(ADDR_NO_RANDOMIZE);
    execute_debugee(prog);
  } else if (pid >= 1) {
    printf("Started debugging process %d\n", pid);
    debugger_t dbg = debugger(prog, pid);
    debugger_run(&dbg);
    debugger_free(&dbg);
  }
}