#ifndef STRING8_H
#define STRING8_H

#include "base.h"

typedef struct {
  B8 *str;
  U64 size;
} String8;

Bool
char_is_space(U8 c);

Bool
char_is_upper(U8 c);

Bool
char_is_lower(U8 c);

Bool
char_is_digit(U8 c);

#endif  // STRING8_H