#include "string8.h"

Bool
char_is_space(U8 c) {
  return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' ||
          c == '\v');
}

Bool
char_is_upper(U8 c) {
  return ('A' <= c && c <= 'Z');
}

Bool
char_is_lower(U8 c) {
  return ('a' <= c && c <= 'z');
}

Bool
char_is_digit(U8 c) {
  return ('0' <= c && c <= '9');
}