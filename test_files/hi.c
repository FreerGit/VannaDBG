#include <stdio.h>

int
main() {
  printf("Starting the program...\n");
  fflush(stdout);

  for (int i = 0; i < 5; i++) {
    printf("Iteration %d\n", i + 1);
    fflush(stdout);
  }

  printf("Finished all iterations.\n");
  fflush(stdout);

  printf("Exiting the program.\n");
  fflush(stdout);
  return 0;
}