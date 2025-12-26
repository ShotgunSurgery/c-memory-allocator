#include "../include/allocator.h"
#include <assert.h>
#include <stdio.h>

int main() {
  void *p = allocate_heap_memory(16);

  // allocator must return a valid pointer
  assert(p != NULL);

  // memory must be writable
  char *c = (char *)p;
  for (int i = 0; i < 16; i++) {
    c[i] = (char)(i + 1);
  }

  // verify written values
  for (int i = 0; i < 16; i++) {
    assert(c[i] == (char)(i + 1));
  }

  printf("test_basic_alloc passed\n");
  return 0;
}
