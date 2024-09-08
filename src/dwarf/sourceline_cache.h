#ifndef SOURCELINE_CACHE_H
#define SOURCELINE_CACHE_H

#include "stdint.h"

typedef struct {
  uintptr_t   start_addr;
  uintptr_t   end_addr;
  const char *filename;
  int         line;
} sourceline_info_t;

#endif  // SOURCELINE_CACHE_H