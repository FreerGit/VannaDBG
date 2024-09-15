#include "base_string8.h"

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

U64
cstring8_len(U8 *c) {
  U8 *p = c;
  while (*p != 0) p += 1;
  return p - c;
}

String8
str8(U8 *str, U64 size) {
  String8 result = {str, size};
  return result;
}

String8
str8_range(U8 *first, U8 *one_past_last) {
  String8 result = {first, (U64)(one_past_last - first)};
  return result;
}

String8
str8_zero() {
  return (String8){0};
}