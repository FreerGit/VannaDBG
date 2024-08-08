#ifndef SLICE_H
#define SLICE_H

#include "assert.h"

/**
 * Use this to implement the functions for a slice type, preferably in the c
 * file to prevent conflicts.
 */
#define SLICE_IMPLEMENTATION(type)                                        \
  type##_slice type##_slice_subslice(type##_slice* slice, uint32_t start, \
                                     uint32_t end) {                      \
    return (type##_slice){slice->ptr + start, end - start};               \
  }                                                                       \
                                                                          \
  type type##_slice_index(type##_slice* slice, uint32_t index) {          \
    assert(index < slice->length && "index out of bounds");               \
    return slice->ptr[index];                                             \
  }

/**
 * Use this to define the functions and the slice type itself.
 */
#define SLICE_DEFINITION(type)                                            \
  typedef struct type##_slice {                                           \
    type*    ptr;                                                         \
    uint32_t length;                                                      \
  } type##_slice;                                                         \
                                                                          \
  type##_slice type##_slice_subslice(type##_slice* slice, uint32_t start, \
                                     uint32_t end);                       \
  type         type##_slice_index(type##_slice* slice, uint32_t index);

#endif  // SLICE_H