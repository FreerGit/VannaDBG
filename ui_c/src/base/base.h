#ifndef BASE_H
#define BASE_H

#include <stdbool.h>
#include <stdint.h>

// Base types
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;
typedef float    F32;
typedef double   F64;
typedef bool     Bool;

// Units
#define KB(x) (((U64)(x)) << 10)
#define MB(x) (((U64)(x)) << 20)
#define GB(x) (((U64)(x)) << 30)
#define TB(x) (((U64)(x)) << 40)

// Vec types

typedef union {
  struct {
    F32 x;
    F32 y;
  };
  F32 v[2];
} Vec2F32;

#endif  // BASE_H