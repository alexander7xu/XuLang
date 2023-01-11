#include <inttypes.h>
#include <stdio.h>

extern int64_t Fib(int64_t);

static int64_t Str2Num(const char *text) {
  int64_t res = 0;
  for (; *text != 0; ++text) {
    int64_t n = *text - '0';
    if (n < 0 || n > 9) return -1;
    res = res * 10 + n;
  }
  return res;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    puts("Usage: ./fib $num");
    return 0;
  }
  int64_t num = Str2Num(argv[1]);
  int64_t res = Fib(num);
  printf("Fibonacci(%" PRId64 ") = (%" PRId64 ")\n", num, res);
  return 0;
}
