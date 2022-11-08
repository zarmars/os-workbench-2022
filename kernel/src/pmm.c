#include <common.h>

heap_mem_type heap_mem;

static void *kalloc(size_t size) {
  mem *r = 0;
  int cpu_id = cpu_current();
  if (size == 0) {
    panic("try to allocate 0 byte.");
    return r;
  }
  acquire(&heap_mem.cpu_mem[cpu_id].proc_lock);
  debug;
  // small memory allocated from l1 cache if it still has free space.
  if (size <= L1_CACHE_NODE_SIZE && heap_mem.cpu_mem[cpu_id].l1_cache) {
    r = heap_mem.cpu_mem[cpu_id].l1_cache;
    heap_mem.cpu_mem[cpu_id].l1_cache = r->next;
    size = L1_CACHE_NODE_SIZE;
    debug;
  } else if (size <= L2_CACHE_NODE_SIZE && heap_mem.cpu_mem[cpu_id].l2_cache) {
    r = heap_mem.cpu_mem[cpu_id].l2_cache;
    heap_mem.cpu_mem[cpu_id].l2_cache = r->next;
    size = L2_CACHE_NODE_SIZE;
  } else {
    int t = 1, n = size + 4 + 4; // size + info + mask(73ba)
    // align to 2^s
    while (n >>= 1)
      t <<= 1;
    t <<= 1;
    printf("t = %d, n = %d\n", t, n);
    acquire(&heap_mem.heap_lock);
    char *s = (char *)heap_mem.shared_mem.start;
    printf("address heap: [%p, %p]\n", heap_mem.shared_mem.start, heap_mem.shared_mem.end);
    int length = 0;
    for (int i = 0; i < heap_mem.shared_mem.end - heap_mem.shared_mem.start; ++i) {
      // printf("s[%d] = %d at %p\n", i, (uint8_t)s[i], (uintptr_t)(s + i));
      panic_on((uint8_t)(s[i]) != 1, "init free value should be 1");
    }
    for (; s + t <= (char*)heap_mem.shared_mem.end;) {
      if ((uint8_t)(*s) == 1) {
        ++length;
        ++s;
      } else {
        length = 0;
        size_t allocate_size;
        memcpy((void *)&allocate_size, (void *)(s + 4), 4);
        // printf("size of %d KiB has been allocated.\n", allocate_size >> 10);
        s = s + 8 + allocate_size;
        panic_on(!IN_RANGE((void*)s, heap_mem.shared_mem),
                 "move out of shared memory.");
      }
      if (length >= t) {
        r = (mem*)(s - (length - 1));
        break;
      }
    }
    printf("length: %d\n", length);
    printf("address of r: %p\n", (uintptr_t)r);
    panic_on(!IN_RANGE((void *)r, heap_mem.shared_mem),
             "r move out of shared memory.");
    if (r) {
      panic_on(length != t, "length != t");
      char *v = (char*)r;
      memset((char *)v, 7, 1);
      v++;
      memset((char *)v, 3, 1);
      v++;
      memset((char *)v, 0xb, 1);
      v++;
      memset((char *)v, 0xa, 1);
      v++;
      size = t - 8;
      memcpy((char *)v, (void *)&size, 4);
      uint32_t check_val;
      memcpy((void *)&check_val, v, 4);
      panic_on(check_val != size, "check_val not equal to size");
      r = (mem*)((char *)v + 4);
      memset((char *)r, 5, size);
    }
    release(&heap_mem.heap_lock);
  }
  debug;
  release(&heap_mem.cpu_mem[cpu_id].proc_lock);
  debug;
  if (r) {
    memset((char *)r, 5, size);
    printf("allocate memory %d byte at %p\n", size, (void*)r);
  }
  debug;
  return (void *)r;
}

static void kfree(void *ptr) {
  int cpu_id = cpu_current();
  printf("cpu_id: %d\n", cpu_id);
  bool is_l1, is_l2, is_l3;
  mem *r;
  if (ptr == 0) {
    printf("Try to free nullptr.\n");
    return;
  }
  is_l1 = IN_RANGE(ptr, heap_mem.cpu_mem[cpu_id].l1_mem) ? true : false;
  is_l2 = IN_RANGE(ptr, heap_mem.cpu_mem[cpu_id].l2_mem) ? true : false;
  is_l3 = IN_RANGE(ptr, heap_mem.shared_mem) ? true : false;
  panic_on(!(is_l1 | is_l2 | is_l3), "Invalid ptr value: Out of Range.");
  if (is_l1) {
    memset(ptr, 1, L1_CACHE_NODE_SIZE);
    // char *s = (char*)ptr;
    // for (int i = 0; i < L1_CACHE_NODE_SIZE; ++i) {
    //   printf("s[%d] = %d at %p\n", i, (uint8_t)s[i], (uintptr_t)(s + i));
    //   panic_on((uint8_t)(s[i]) != 1, "init free value should be 1");
    // }
    printf("ptr: %p\n", (uintptr_t)ptr);
    r = (mem *)ptr;
    printf("size of ptr: %d\n", sizeof(r));
    printf("ptr: %p\n", (uintptr_t)r);
    acquire(&heap_mem.cpu_mem[cpu_id].proc_lock);
    r->next = heap_mem.cpu_mem[cpu_id].l1_cache;
    heap_mem.cpu_mem[cpu_id].l1_cache = r;
    printf("free memory %d byte at %p\n", L1_CACHE_NODE_SIZE, (void*)r);
    release(&heap_mem.cpu_mem[cpu_id].proc_lock);
  } else if (is_l2) {
    memset(ptr, 1, L2_CACHE_NODE_SIZE);
    r = (mem *)ptr;
    acquire(&heap_mem.cpu_mem[cpu_id].proc_lock);
    r->next = heap_mem.cpu_mem[cpu_id].l2_cache;
    heap_mem.cpu_mem[cpu_id].l2_cache = r;
    printf("free memory %d byte at %p\n", L2_CACHE_NODE_SIZE, (void*)r);
    release(&heap_mem.cpu_mem[cpu_id].proc_lock);
  } else {
    acquire(&heap_mem.heap_lock);
    char *s = ptr - 8;
    panic_on(!((uint8_t)s[0] == 7 && (uint8_t)s[1] == 3 && (uint8_t)s[2] == 0xb &&
                 (uint8_t)s[3] == 0xa),
             "detect invalid memory segment to free.");
    uint32_t size;
    memcpy((void *)&size, ptr - 4, 4);
    memset(s, 1, 8);
    memset((char *)ptr, 1, size);
    printf("free memory %d byte at %p\n", size, (void*)ptr);
    release(&heap_mem.heap_lock);
  }
}

static void mem_init();

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, (uintptr_t)heap.start, (uintptr_t)heap.end);
  mem_init();
}

MODULE_DEF(pmm) = {
    .init = pmm_init,
    .alloc = kalloc,
    .free = kfree,
};

static void mem_init() {
  spinlock cpu_lock[MAX_CPU];
  initlock(&heap_mem.heap_lock, "share memory lock.");
  debug;
  for (int i = 0; i < MAX_CPU; i++) {
    heap_mem.cpu_mem[i] = (proc_mem_type){
        .l1_cache = 0,
        .l2_cache = 0,
        .l1_mem = {},
        .l2_mem = {},
        .local_mem =
            RANGE(ROUNDUP(heap.start + CPU_MEM_SIZE * i, PGSIZE),
                  ROUNDDOWN(heap.start + CPU_MEM_SIZE * (i + 1), PGSIZE)),
        .proc_lock = cpu_lock[i]};
    initlock(&heap_mem.cpu_mem[i].proc_lock, TOSTRING(i));
  }
  if (ROUNDUP(heap.start + CPU_MEM_SIZE * MAX_CPU, PGSIZE) <
      ROUNDDOWN(heap.end, PGSIZE)) {
    heap_mem.shared_mem =
        RANGE(ROUNDUP(heap.start + CPU_MEM_SIZE * MAX_CPU, PGSIZE),
              ROUNDDOWN(heap.end, PGSIZE));
  } else {
    //    heap_mem.shared_mem = RANGE(heap.end, heap.end);
    panic("no shared memory left.");
  }
  debug;
  initcache();
}

// cpu owned memory will be all set as 1 if they are free, and 5 if they are
// allocated. shared memory in heap will be set as 3 if they are free, and 7 if
// they are allocated.
void initcache() {
  for (int i = 0; i < MAX_CPU; i++) {
    void *mem_start = heap_mem.cpu_mem[i].local_mem.start;
    heap_mem.cpu_mem[i].l1_mem =
        RANGE(mem_start, mem_start + L1_CACHE_TOTAL_SIZE);
    freerange(&heap_mem.cpu_mem[i].l1_cache, heap_mem.cpu_mem[i].l1_mem,
              L1_CACHE_NODE_SIZE, 1);
    heap_mem.cpu_mem[i].l2_mem =
        RANGE(mem_start + L1_CACHE_TOTAL_SIZE,
              mem_start + L1_CACHE_TOTAL_SIZE + L2_CACHE_TOTAL_SIZE);
    freerange(&heap_mem.cpu_mem[i].l2_cache, heap_mem.cpu_mem[i].l2_mem,
              L2_CACHE_NODE_SIZE, 1);
  }
  panic_on(heap_mem.shared_mem.start >= heap_mem.shared_mem.end,
           "no shared memory left.");
  memset(heap_mem.shared_mem.start, 1, (size_t)(heap_mem.shared_mem.end - heap_mem.shared_mem.start));
  debug;
}

// Don't need to consider data race during execution of os_init.
void freerange(mem **freelist, Area a, size_t unit_size, int fill_val) {
  char *p;
  mem *r;
  panic_on(freelist == 0, "null freelist.");
  panic_on(unit_size <= 0, "no memory left.");
  panic_on(a.start >= a.end, "invalid memory area.");
  p = (char *)(a.start);
  for (; p + unit_size <= (char*)a.end; p += unit_size) {
    memset(p, fill_val, unit_size);
    r = (mem *)p;
    r->next = *freelist;
    *freelist = r;
  }
}

void kfree_helper(void *pa, int fill_val) {
  int cpu_id = cpu_current();
  panic_on(IN_RANGE(pa, heap_mem.cpu_mem[cpu_id].local_mem),
           "kfree memory out of range.");
  bool is_l1_cache = (bool)IN_RANGE(pa, heap_mem.cpu_mem[cpu_id].l1_mem);
  size_t mem_size = is_l1_cache ? L1_CACHE_NODE_SIZE : L2_CACHE_NODE_SIZE;
  acquire(&heap_mem.cpu_mem[cpu_id].proc_lock);
  // fill with junk w.r.t each cpu id to catch dangling refs.
  memset(pa, cpu_id + 1, mem_size);
  mem *r = (mem *)pa;
  if (is_l1_cache) {
    r->next = heap_mem.cpu_mem[cpu_id].l1_cache;
    heap_mem.cpu_mem[cpu_id].l1_cache = r;
  } else {
    r->next = heap_mem.cpu_mem[cpu_id].l2_cache;
    heap_mem.cpu_mem[cpu_id].l2_cache = r;
  }
  release(&heap_mem.cpu_mem[cpu_id].proc_lock);
}
