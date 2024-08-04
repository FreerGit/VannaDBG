typedef struct {
  long long    abc;
  unsigned int def;
} abc_t;

int
main() {
  long a  = 3;
  long b  = 2;
  long c  = a + b;
  a       = 4;
  abc_t x = {};
}