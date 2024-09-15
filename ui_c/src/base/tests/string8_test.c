
#include <string.h>

#include "base/base_inc.c"
#include "base/base_testlib.h"

TEST_INIT_GLOBAL();

S32
main() {
  // Character expect
  TEST_ASSERT(char_is_space(' '));
  TEST_ASSERT(char_is_space('\n'));
  TEST_ASSERT(!char_is_space('a'));

  TEST_ASSERT(char_is_upper('A'));
  TEST_ASSERT(char_is_upper('B'));
  TEST_ASSERT(char_is_upper('Z'));
  TEST_ASSERT(!char_is_upper('u'));
  TEST_ASSERT(!char_is_upper(';'));

  TEST_ASSERT(char_is_lower('u'));
  TEST_ASSERT(char_is_lower('a'));
  TEST_ASSERT(char_is_lower('z'));
  TEST_ASSERT(!char_is_lower('G'));

  TEST_ASSERT(char_is_digit('0'));
  TEST_ASSERT(char_is_digit('1'));
  TEST_ASSERT(char_is_digit('4'));
  TEST_ASSERT(char_is_digit('9'));
  TEST_ASSERT(!char_is_digit('A'));

  // C-string
  U8 c[] = "A C-string";
  TEST_ASSERT_EQL(cstring8_len(c), 10);

  // String8 constructors
  TEST_ASSERT_EQL(str8_lit("A static cstring").size, 16);
  String8 str = str8(c, 10);
  TEST_ASSERT_EQL(str.size, 10)
  TEST_ASSERT_EQL(strcmp((char*)str.str, (char*)c), 0);

  String8 s = str8_lit_comp("A static cstring");

  TEST_ASSERT(s.size == 16);

  String8 str_range = str8_range(&c[2], &c[8]);
  TEST_ASSERT_EQL(strcmp((char*)str_range.str, "C-string"), 0);
  TEST_ASSERT_EQL(str_range.size, 6);

  TEST_ASSERT_EQL(str8_zero().size, 0);
  TEST_ASSERT_EQL(str8_zero().str, 0);

  TEST_RESULT();
}