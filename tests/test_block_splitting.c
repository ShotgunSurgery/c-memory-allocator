#include "../include/allocator.h"
#include <stdio.h>

int main() {
  void *a = allocate_heap_memory(16);
  void *b = allocate_heap_memory(16);
  void *c = allocate_heap_memory(16);

  printf("a = %p\n", a);
  printf("b = %p\n", b);
  printf("c = %p\n", c);

  // These must all be non-NULL
  if (!a || !b || !c) {
    printf("Allocation failed unexpectedly\n");
    return 1;
  }

  // They must not overlap
  if (a == b || b == c || a == c) {
    printf("Overlapping allocations detected\n");
    return 1;
  }

  printf("PASS: basic block splitting test\n");
  return 0;
}
