#ifndef BASE_STRING8_H
#define BASE_STRING8_H

#include "base.h"

typedef struct {
  U8 *str;
  U64 size;
} String8;

// Character expect

Bool
char_is_space(U8 c);

Bool
char_is_upper(U8 c);

Bool
char_is_lower(U8 c);

Bool
char_is_digit(U8 c);

// C-string
U64
cstring8_len(U8 *c);

// String8 constructors
#define str8_lit(S) str8((U8 *)(S), sizeof(S) - 1)
#define str8_lit_comp(S) \
  { (U8 *)(S), sizeof(S) - 1, }

String8
str8(U8 *str, U64 size);

String8
str8_range(U8 *first, U8 *one_past_last);

String8
str8_zero();

#endif  // BASE_STRING8_H