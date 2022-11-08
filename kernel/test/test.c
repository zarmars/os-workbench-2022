#include "thread.h"


// test memmory structure
static void test_0() {
}

// smoke test: malloc and free within signle thread
static void test_1() {
}

// single-cpu multi-thread malloc & free
static void test_2() {
}

// pressure-test: single-cpu multi-thread frequently malloc & free small or big
// memmory.
static void test_3() {
}

int main(int argc, char *argv[]) {
  if (argc != 2) return 1;
  char* s = argv[1];
  if (s == 0 || s[1] != '\0') return 1;
  int test = (int)(s[0] - '0');
  switch (test) {
    case 0:
      test_0();
      break;
    case 1:
      test_1();
      break;
    case 2:
      test_2();
      break;
    case 3:
      test_3();
      break;
    default:
      break;
  }
  return 0;
}
