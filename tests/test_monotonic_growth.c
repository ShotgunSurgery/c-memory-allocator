#include "../include/allocator.h"
#include <stdint.h>
#include <stdio.h>

int main() {
  void *a = allocate_heap_memory(16);
  void *b = allocate_heap_memory(16);
  void *c = allocate_heap_memory(16);

  if (!a || !b || !c) {
    printf("FAIL: NULL allocation\n");
    return 1;
  }

  if (!(a < b && b < c)) {
    printf("FAIL: allocations are not monotonically increasing\n");
    printf("a=%p b=%p c=%p\n", a, b, c);
    return 1;
  }

  printf("PASS: monotonic growth test\n");
  return 0;
}
