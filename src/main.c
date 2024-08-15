#include <gelf.h>
#include <libelf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/personality.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include "core/debugger.h"
#include "dwarf/conversion.h"
#include "ui/ui.h"

void
execute_debugee(char *prog_name) {
  if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
    printf("Error in ptrace\n");
    return;
  }

  execl(prog_name, prog_name, NULL);
  perror("execl");
  exit(1);
}

int
main() {
  int pid = fork();
  if (pid == 0) {
    // TODO(improvement): ASLR is enabled for now, this should be user definable
    personality(ADDR_NO_RANDOMIZE);
    execute_debugee("./test_files/a.out");
  } else if (pid >= 1) {
    int status;
    waitpid(pid, &status, 0);

    // uintptr_t base_addr = get_base_address(pid);
    uintptr_t main_addr = find_main_address(pid);

    printf("Address of main: 0x%lx\n", main_addr);

    printf("Base addr: 0x%lx\n", get_base_address(pid));

    if (WIFEXITED(status)) {
      printf("Child exited\n");
      return -1;
    }
    if (!WIFSTOPPED(status)) {
      printf("Unexpected status\n");
      return -1;
    }

    printf("Started debugging process %d\n", pid);

    FILE *fp = fopen("./test_files/a.out", "r");
    int   fd = fileno(fp);
    assert(fp != NULL);
    assert(fd != 0);

    arena_t vanna_arena = arena_create(1024 * 1024 * 1024);

    arena_t    dbg_arena = arena_scratch(&vanna_arena, 1024 * 1024 * 8);
    debugger_t dbgr      = debugger(&dbg_arena, "./test_files/a.out", pid);

    // TODO just set a breakpoint for now, later on the dbg will have need a
    // file window of some sort
    // breakpoint_create(pid, "name");

    // debugger_run(&dbg);
    // debugger_free(&dbg);

    int res = start_ui(&dbgr);
    return res;
  }
}