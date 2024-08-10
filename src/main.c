#include <gelf.h>
#include <libelf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/personality.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include "core/debugger.h"
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

uintptr_t
find_main_address(pid_t pid) {
  char proc_exe[64];
  snprintf(proc_exe, sizeof(proc_exe), "/proc/%d/exe", pid);

  FILE *fp = fopen(proc_exe, "r");
  int   fd = fileno(fp);
  if (fd < 0) {
    perror("Failed to open executable");
    return 0;
  }

  if (elf_version(EV_CURRENT) == EV_NONE) {
    fprintf(stderr, "ELF library initialization failed\n");
    close(fd);
    return 0;
  }

  Elf *elf = elf_begin(fd, ELF_C_READ, NULL);
  if (!elf) {
    fprintf(stderr, "elf_begin() failed\n");
    close(fd);
    return 0;
  }

  GElf_Ehdr ehdr;
  if (gelf_getehdr(elf, &ehdr) == NULL) {
    fprintf(stderr, "gelf_getehdr() failed\n");
    elf_end(elf);
    close(fd);
    return 0;
  }

  Elf_Scn  *scn = NULL;
  GElf_Shdr shdr;
  Elf_Data *data;
  int       symbol_count;

  while ((scn = elf_nextscn(elf, scn)) != NULL) {
    gelf_getshdr(scn, &shdr);
    if (shdr.sh_type == SHT_SYMTAB) {
      data         = elf_getdata(scn, NULL);
      symbol_count = shdr.sh_size / shdr.sh_entsize;

      for (int i = 0; i < symbol_count; i++) {
        GElf_Sym sym;
        gelf_getsym(data, i, &sym);

        char *name = elf_strptr(elf, shdr.sh_link, sym.st_name);
        if (name && strcmp(name, "main") == 0) {
          elf_end(elf);
          close(fd);
          return (uintptr_t)sym.st_value;
        }
      }
    }
  }

  elf_end(elf);
  close(fd);
  return 0;
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

    uintptr_t          main_addr = find_main_address(pid);
    unsigned long long base_addr = 0x555555554000;
    printf("Address of main: 0x%llx\n", main_addr + base_addr);

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

    debugger_t dbgr = debugger("./test_files/a.out", pid);

    // debugger_run(&dbg);
    // debugger_free(&dbg);

    int res = start_ui(&dbgr);
    return res;
  }
}