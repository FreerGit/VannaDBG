#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "../ext/linenoise/linenoise.h"

typedef struct {
  char* prog_name;
  int   pid;
} debugger_t;

char**
split(char* str, char* delim, size_t* i) {
  char** out   = malloc(256 * sizeof(char*));
  char*  token = strtok(str, delim);
  while (token != NULL) {
    out[(*i)++] = token;
    token       = strtok(NULL, delim);
  }
  return out;
}

void
continue_execution(debugger_t* dbg) {
  ptrace(PTRACE_CONT, dbg->pid, NULL, NULL);
  int wait_status;
  int options = 0;
  waitpid(dbg->pid, &wait_status, options);
}

void
handle_command(debugger_t* dbg, char* line) {
  size_t size    = 0;
  char** args    = split(line, " ", &size);
  char*  command = args[0];

  if (strcmp(command, "cont") == 0) {
    continue_execution(dbg);
  } else {
    fprintf(stderr, "Unknown command: %s\n", command);
  }

  free(args);
}

void
execute_debugee(char* prog_name) {
  if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
    printf("Error in ptrace\n");
    return;
  }
  execl(prog_name, prog_name, NULL);
}

void
run(debugger_t* dbg) {
  int wait_status;
  int options = 0;

  char* line = NULL;
  while ((line = linenoise("VannaDBG> ")) != NULL) {
    handle_command(dbg, line);
    linenoiseHistoryAdd(line);
    linenoiseFree(line);
  }
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
    execute_debugee(prog);
  } else if (pid >= 1) {
    printf("Started debugging process\n");
    debugger_t dbg = {prog, pid};
    run(&dbg);
  }
}