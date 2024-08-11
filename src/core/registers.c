#include "registers.h"

#include <stddef.h>
#include <string.h>

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