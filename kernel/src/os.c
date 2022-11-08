#include <common.h>

static void os_init() {
  pmm->init();
}

static void user_entry() {
  debug;
  // test->test_0();
  // test->test_1();
  test->test_2();
  test->test_3();
}

static void os_run() {
  for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
  user_entry();
  while (1) ;
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
