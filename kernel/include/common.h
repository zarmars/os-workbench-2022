#include <kernel.h>
#include <klib.h>
#include <klib-macros.h>

#define MAX_CPU 8
#define PGSIZE 4096
// For simplicity, currently use hard-code defined memory size,
// with assumption that total heap memory bigger than 64MiB
#define L1_CACHE_NODE_SIZE 128
#define L1_CACHE_NODE_NUM 8192
// L1 cache: 1MiB
#define L1_CACHE_TOTAL_SIZE (L1_CACHE_NODE_SIZE * L1_CACHE_NODE_NUM)
#define L2_CACHE_NODE_SIZE (PGSIZE)
#define L2_CACHE_NODE_NUM 1792
// L2 cache: 7MiB
#define L2_CACHE_TOTAL_SIZE (L2_CACHE_NODE_SIZE * L2_CACHE_NODE_NUM)

#define CPU_MEM_SIZE (L1_CACHE_TOTAL_SIZE + L2_CACHE_TOTAL_SIZE)

#define AREA_LENGTH(area) ((area).end - (area).start)

#define mb() asm volatile ("mfence" ::: "memory")

// #define debug (printf("reach %s:%s\n", __FILE__, TOSTRING(__LINE__)));

#define debug


typedef struct {
  int locked;    // locked status: 0(not held), 1(held)
  // debug info
  char* name;
} spinlock;

// list-organized memory,
struct mem{
  struct mem* next;
};

typedef struct mem mem;

typedef struct {
  // list containing small memory pieces, each node has 128 bytes, total 256
  // bytes.
  mem *l1_cache;
  // list containing medium memory pieces, each node has 4K bytes, total 8Mib
  // 256bytes.
  mem *l2_cache;
  // process local heap memory.
  Area l1_mem;
  Area l2_mem;
  Area local_mem;
  // If need some large memory, eg., 10MB, directly borrow from
  // total heap, and free them after use. Hence, memory address
  // that does't within local_mem comes from total heap area.
  // ...
  // local-lock
  spinlock proc_lock;

} proc_mem_type;

// typedef struct {
//   Area seg;
//   Arealist *next;
//   Arealist *prev;
// } Arealist;

typedef struct {
  proc_mem_type cpu_mem[MAX_CPU];    // local process owned memory.
//  Arealist l3_cache;                // memory shared by all processes.
  Area shared_mem;
  spinlock heap_lock;                // lock for shared memory.
} heap_mem_type;

extern heap_mem_type heap_mem;

extern volatile bool intr_enabled;

void initlock(spinlock *lk, char *name);

int holding(spinlock *lk);

void acquire(spinlock *lk);

void release(spinlock *lk);

void initcache();

void freerange(mem** head, Area a, size_t unit_size, int fill_val);

void kfree_helper(void *pa, int fill_val);
