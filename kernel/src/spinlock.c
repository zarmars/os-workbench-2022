#include <common.h>

volatile bool intr_enabled = true;

void initlock(spinlock *lk, char *name) {
  lk->name = name;
  lk->locked = 0;
}

int holding(spinlock *lk) {
  return (int)(lk->locked);
}

void acquire(spinlock *lk) {
  // iset(false);   // disable interrupts to avoid deadlock.
  intr_enabled = ienabled();
  iset(false);
  if (holding(lk)) {
    panic("acquire");
  }
  while(atomic_xchg(&lk->locked, 1) != 0)
    ;
  
  mb();
}

void release(spinlock *lk) {
  if (!holding(lk)) {
    panic("release");
  }
  mb();

  atomic_xchg(&lk->locked, 0);
  if (intr_enabled) {
    iset(true);
  }
  // iset(true);
}
