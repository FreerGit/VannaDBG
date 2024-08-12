#ifndef CONVERSION_H
#define CONVERSION_H

#include <stdint.h>
#include <sys/wait.h>

uintptr_t
find_main_address(pid_t pid);

uintptr_t
get_base_address(pid_t pid);

uintptr_t
find_address_by_line(const char *filename, const char *source_file,
                     int line_number);

#endif  // CONVERSION_H