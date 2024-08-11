#include <stdio.h>

void
a() {
  printf("Starting the program...\n");
  fflush(stdout);
}

void
b() {
  for (int i = 0; i < 5; i++) {
    printf("Iteration %d\n", i + 1);
    fflush(stdout);
  }
}

void
c() {
  printf("Finished all iterations.\n");
  fflush(stdout);
}

void
d() {
  printf("Exiting the program.\n");
  fflush(stdout);
}

int
main() {
  a();
  b();
  c();
  d();
}