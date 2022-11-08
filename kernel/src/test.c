#include <common.h> 

void alloc_check(void *p, size_t cache_flag) {
  int cpu = cpu_current();
  proc_mem_type *cpu_mem = &heap_mem.cpu_mem[cpu];
  switch (cache_flag) {
  case 1: {
    panic_on(!IN_RANGE(p, cpu_mem->l1_mem), "p start should in l1 cache range");
    panic_on(!IN_RANGE((void*)(p + L1_CACHE_NODE_SIZE - 1), cpu_mem->l1_mem),
             "p end should in l1 cache range");
    char *s = (char *)p;
    for (int i = 0; i < L1_CACHE_NODE_SIZE; ++i) {
      panic_on((uint8_t)(s[i]) != 5, "init alloc value should be 5");
    }
    break;
  }
  case 2: {
    panic_on(!IN_RANGE(p, cpu_mem->l2_mem), "p start should in l2 cache range");
    panic_on(!IN_RANGE((void*)(p + L2_CACHE_NODE_SIZE - 1), cpu_mem->l2_mem),
             "p end should in l2 cache range");
    char *s = (char *)p;
    for (int i = 0; i < L2_CACHE_NODE_SIZE; ++i) {
      panic_on((uint8_t)(s[i]) != 5, "init alloc value should be 5");
    }
    break;
  }
  case 3: {
    panic_on(!IN_RANGE(p, heap_mem.shared_mem),
             "p start should in l3 cache range");

    char *s = (char *)p - 8;
    for (int i = 0; i < 8; ++i) {
      // printf("s[%d] = %d\n", i, (uint8_t)s[i]);
    }
    panic_on(!((uint8_t)s[0] == 7 && (uint8_t)s[1] == 3 && (uint8_t)s[2] == 0xb &&
                 (uint8_t)s[3] == 0xa),
             "detect invalid memory segment to free.");
    size_t size;
    memcpy((void *)&size, p - 4, 4);
    // printf("size: %x, p: %p, start: %p, end: %p\n", size, p,
    // heap_mem.shared_mem.start, heap_mem.shared_mem.end); if
    // ((void*)(p+size-1) >= heap_mem.shared_mem.start) {
    //   printf("Duby\n");
    //   if ((void*)(p+size-1) < heap_mem.shared_mem.end) {
    //     printf("Miko\n");
    //   } else {
    //     printf("size of p:%d\n", sizeof(p));
    //     printf("size: %x, p:%p, %p:%p\n",
    //     (void*)(p)+size-1-heap_mem.shared_mem.start, p+1, (void*)(p)+size-1,
    //     heap_mem.shared_mem.end); printf("%d, %d\n",
    //     (void*)(p+size-1)-heap_mem.shared_mem.start,
    //     heap_mem.shared_mem.end-heap_mem.shared_mem.start);
    //   }
    // }
    panic_on(!IN_RANGE((void*)(p + size - 1), heap_mem.shared_mem),
             "p end should in l3 cache range");
    // printf("%d:%d\n", heap_mem.shared_mem.end - heap_mem.shared_mem.start, (p - heap_mem.shared_mem.start) + size - 1);
    // size_t volatile mem_length = heap_mem.shared_mem.end - heap_mem.shared_mem.start;
    // size_t volatile allo_length_from_start = (p - heap_mem.shared_mem.start) + size - 1;
    // printf("mem_length: %d, all_length: %d\n", mem_length, allo_length_from_start);
    // panic_on(67108864<=4194304, "fuck");
    // if ((int64_t)mem_length <= (int64_t)allo_length_from_start) {
    //   printf("Are you kidding me? \n");
    // }
    // panic_on(mem_length <= allo_length_from_start, "p end should in l3 cache range");
    printf("found allocate size: %d\n", size);
    s = p;
    for (int i = 0; i < size; ++i) {
      if ((uint8_t)s[i] != 5) {
        printf("s[%d] = %d\n", i, (uint8_t)s[i]);
      }
      panic_on((uint8_t)(s[i]) != 5, "init alloc value should be 5");
    }
    break;
  }
  default:
    break;
  }
}

void free_check(void *p, size_t size, size_t cache_flag) {
  int cpu = cpu_current();
  proc_mem_type *cpu_mem = &heap_mem.cpu_mem[cpu];
  switch (cache_flag) {
  case 1: {
    panic_on(!IN_RANGE(p, cpu_mem->l1_mem), "p start should in l1 cache range");
    panic_on(!IN_RANGE((void*)(p + L1_CACHE_NODE_SIZE - 1), cpu_mem->l1_mem),
             "p end should in l1 cache range");
    char *s = (char *)p;
    for (int i = sizeof(mem*); i < L1_CACHE_NODE_SIZE; ++i) {
//      printf("s[%d], %d\n", i, (uint8_t)s[i]);
      panic_on((uint8_t)(s[i]) != 1, "init free value should be 1");
    }
    break;
  }
  case 2: {
    panic_on(!IN_RANGE(p, cpu_mem->l2_mem), "p start should in l2 cache range");
    panic_on(!IN_RANGE((void*)(p + L2_CACHE_NODE_SIZE - 1), cpu_mem->l2_mem),
             "p end should in l2 cache range");
    char *s = (char *)p;
    for (int i = sizeof(mem*); i < L2_CACHE_NODE_SIZE; ++i) {
      panic_on((uint8_t)(s[i]) != 1, "init free value should be 1");
    }
    break;
  }
  case 3: {
    panic_on(!IN_RANGE(p, heap_mem.shared_mem),
             "p start should in l3 cache range");
    char *s = (char *)p - 8;
    // size_t size;
    // memcpy((void *)&size, p - 4, 4);
    printf("free_check size = %d, p end = %p\n", size, (uintptr_t)(p + size - 1));
    panic_on(!IN_RANGE((void*)(p + size - 1), heap_mem.shared_mem),
             "p end should in shared memory range");
    for (int i = 0; i < size + 8; ++i) {
      panic_on((uint8_t)(s[i]) != 1, "init free value should be 1");
    }
    break;
  }
  default:
    break;
  }
}

// test small memmory allocation and free.
static void test_0() {
  printf("run test_0 in cpu[%d]...\n", cpu_current());
  debug;
  void *p1 = pmm->alloc(128);
  alloc_check(p1, 1);
  debug;

  void *p2 = pmm->alloc(36);
  alloc_check(p2, 1);

  pmm->free(p1);
  free_check(p1, L1_CACHE_NODE_SIZE, 1);

  pmm->free(p2);
  free_check(p2, L1_CACHE_NODE_SIZE, 1);

  p1 = pmm->alloc(67);
  alloc_check(p1, 1);

  pmm->free(p1);
  free_check(p1, L1_CACHE_NODE_SIZE, 1);

  p2 = pmm->alloc(1);
  alloc_check(p2, 1);

  pmm->free(p2);
  free_check(p2, L1_CACHE_NODE_SIZE, 1);

  printf("test_0 passed at cpu[%d].\n", cpu_current());
}

// medium memory allocation and free.
static void test_1() {
  printf("run test_1 at cpu[%d]...\n", cpu_current());

  void *p[L2_CACHE_NODE_NUM];
  for (int i = 0; i < L2_CACHE_NODE_NUM; ++i) {
    size_t size = (i % 3) + 1;
    p[i] = pmm->alloc(L2_CACHE_NODE_SIZE / size);
    alloc_check(p[i], 2);
  }
  for (int i = 0; i < L2_CACHE_NODE_NUM; ++i) {
    pmm->free(p[i]);
    free_check(p[i], L2_CACHE_NODE_SIZE, 2);
  }

  printf("test_1 passed at cpu[%d].\n", cpu_current());
}

// L1-cache used up, take L2-cache
static void test_2() {
  printf("run test_2 at cpu[%d]...\n", cpu_current());

  void *p[L1_CACHE_NODE_NUM];
  for (int i = 0; i < L1_CACHE_NODE_NUM; ++i) {
    size_t size = (i % 3) + 1;
    p[i] = pmm->alloc(L1_CACHE_NODE_SIZE / size);
    alloc_check(p[i], 1);
  }
  void *p1 = pmm->alloc(3);
  alloc_check(p1, 2);
  pmm->free(p1);
  free_check(p1, L2_CACHE_NODE_SIZE, 2);
  for (int i = 0; i < L1_CACHE_NODE_NUM; ++i) {
    pmm->free(p[i]);
    free_check(p[i], L1_CACHE_NODE_SIZE, 1);
  }

  printf("test_2 passed at cpu[%d].\n", cpu_current());
}

// big memory allocation and free.
static void test_3() {
  printf("run test_3 at cpu[%d]...\n", cpu_current());

  size_t size[5] = {1, 2, 3, 4, 5};
  for (int i = 0; i < 15; ++i) {
    size_t allocate_size = (size[i % 5]) << 20;
    void *p = pmm->alloc(allocate_size);
    alloc_check(p, 3);
    pmm->free(p);
    free_check(p, allocate_size, 3);
  }

  printf("test_3 passed at cpu[%d].\n", cpu_current());
}

MODULE_DEF(test) = {
    .test_0 = test_0, .test_1 = test_1, .test_2 = test_2, .test_3 = test_3};
