#include "../include/allocator.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct block_header {
  size_t size_of_block;
  bool allocation_status;
  bool previous_block_status;
} block_header;

int main() {
  void *a = allocate_heap_memory(32);
  void *b = allocate_heap_memory(32);

  if (!a || !b) {
    printf("FAIL: NULL allocation\n");
    return 1;
  }

  // Header is immediately before payload
  block_header *h1 = (block_header *)((char *)a - sizeof(block_header));
  block_header *h2 = (block_header *)((char *)b - sizeof(block_header));

  // First block must be allocated
  if (!h1->allocation_status) {
    printf("FAIL: first block not marked allocated\n");
    return 1;
  }

  // Second block header must be valid
  if (h2->size_of_block == 0 || h2->size_of_block > HEAP_SIZE) {
    printf("FAIL: second block header corrupted (size=%zu)\n", h2->size_of_block);
    return 1;
  }

  printf("PASS: header sanity test\n");
  return 0;
}
