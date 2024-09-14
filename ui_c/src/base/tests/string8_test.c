#include "../string8.h"

#include <stdio.h>

#include "../libtest.h"
#include "../string8.c"

TEST_INIT_GLOBAL();

S32
main() {
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

  TEST_RESULT();
}