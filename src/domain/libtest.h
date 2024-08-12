#ifndef LIBTEST_H
#define LIBTEST_H

#include <stdio.h>

#define TEST_INIT_GLOBAL() \
  int global_total_tests;  \
  int global_failed_tests;

#define TEST_RESULT()                                                   \
  int   result = (global_failed_tests != 0);                            \
  char  buffer[1024];                                                   \
  char* color = result ? "\x1B[31m" : "\x1B[32m";                       \
  char* conf  = result ? "Failed" : "Successful";                       \
  sprintf(buffer, "%s%s %s", color, __FILE__, conf);                    \
  printf("%s %d/%d passed.\e[0m\n", buffer,                             \
         global_total_tests - global_failed_tests, global_total_tests); \
  return result ? -1 : 0;

#define TEST_ASSERT(expression)                                        \
  ++global_total_tests;                                                \
  if (!(expression)) {                                                 \
    ++global_failed_tests;                                             \
    printf("\x1B[31m%s(%d): test failed.\e[0m\n", __FILE__, __LINE__); \
  }

// parameters must be comparable, ie a == b.
#define TEST_ASSERT_EQL(a, b)                                          \
  ++global_total_tests;                                                \
  if (!(a == b)) {                                                     \
    ++global_failed_tests;                                             \
    printf("\x1B[31m%s(%d): test failed.\e[0m\n", __FILE__, __LINE__); \
  }

#endif  // LIBTEST_H