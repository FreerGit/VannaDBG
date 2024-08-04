#ifndef REGISTERS_H
#define REGISTERS_H

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>

#include "hashmap.h"

typedef enum {
  rax,
  rbx,
  rcx,
  rdx,
  rdi,
  rsi,
  rbp,
  rsp,
  r8,
  r9,
  r10,
  r11,
  r12,
  r13,
  r14,
  r15,
  rip,
  rflags,
  cs,
  orig_rax,
  fs_base,
  gs_base,
  fs,
  gs,
  ss,
  ds,
  es,
} reg_t;

constexpr size_t n_registers = 27;

typedef struct {
  reg_t r;
  int   dwarf_r;
  char *name;
} reg_description_t;

static const reg_description_t reg_desc_array[n_registers] = {
    {r15,      15, "r15"     },
    {r14,      14, "r14"     },
    {r13,      13, "r13"     },
    {r12,      12, "r12"     },
    {rbp,      6,  "rbp"     },
    {rbx,      3,  "rbx"     },
    {r11,      11, "r11"     },
    {r10,      10, "r10"     },
    {r9,       9,  "r9"      },
    {r8,       8,  "r8"      },
    {rax,      0,  "rax"     },
    {rcx,      2,  "rcx"     },
    {rdx,      1,  "rdx"     },
    {rsi,      4,  "rsi"     },
    {rdi,      5,  "rdi"     },
    {orig_rax, -1, "orig_rax"},
    {rip,      -1, "rip"     },
    {cs,       51, "cs"      },
    {rflags,   49, "eflags"  },
    {rsp,      7,  "rsp"     },
    {ss,       52, "ss"      },
    {fs_base,  58, "fs_base" },
    {gs_base,  59, "gs_base" },
    {ds,       53, "ds"      },
    {es,       50, "es"      },
    {fs,       54, "fs"      },
    {gs,       55, "gs"      },
};

// TODO these really should be a switch statement.

uint64_t
get_register_value(pid_t pid, reg_t r) {
  struct user_regs_struct regs;
  ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
  uint64_t *regs_array = (uint64_t *)&regs;

  for (size_t i = 0; i < n_registers; i++) {
    if (reg_desc_array[i].r == r) {
      return regs_array[i];
    }
  }
  unreachable();
}

void
set_register_value(pid_t pid, reg_t r, uint64_t value) {
  struct user_regs_struct regs;
  ptrace(PTRACE_GETREGS, pid, NULL, &regs);

  uint64_t *regs_array = (uint64_t *)&regs;

  for (size_t i = 0; i < n_registers; i++) {
    if (reg_desc_array[i].r == r) {
      regs_array[i] = value;
      break;
    }
  }

  ptrace(PTRACE_SETREGS, pid, NULL, &regs);
}

uint64_t
get_register_value_from_dwarf_register(pid_t pid, int regnum) {
  for (size_t i = 0; i < n_registers; i++) {
    if (reg_desc_array[i].dwarf_r == regnum) {
      return get_register_value(pid, reg_desc_array[i].r);
    }
  }
  assert(0 && "Unknown dwarf register");
}

const char *
get_register_name(reg_t r) {
  for (size_t i = 0; i < n_registers; i++) {
    if (reg_desc_array[i].r == r) {
      return reg_desc_array[i].name;
    }
  }
  assert(0 && "Could not find name of register");
  // return NULL;
}

reg_t
get_register_from_name(const char *name) {
  for (size_t i = 0; i < n_registers; i++) {
    if (strcmp(reg_desc_array[i].name, name) == 0) {
      return reg_desc_array[i].r;
    }
  }
  assert(0 && "Could not find register by name");
  // return NULL;
}

#endif  // REGISTERS_H