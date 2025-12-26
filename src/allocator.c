#include "../include/allocator.h"
#include <stdbool.h>
#include <stdint.h>

// defining heap
unsigned char heap[HEAP_SIZE];
bool heap_initiated = 0;

// defining header for memory blocks with data
typedef struct block_header {
  size_t size_of_block;
  bool allocation_status;
  bool previous_block_status;
} block_header;

// defining header for block that is free and is a part of free list
typedef struct free_list_block_header {
  size_t size;
  bool allocation_status;
  bool previous_block_status;
  void *forward_ptr;
  void *previous_ptr;
} free_list_block_header;

// declaring first, last blocks 
block_header *first;
block_header *epilogue_block;

block_header *previous_block_ptr;

// initilizing the heap
void initiate_heap() {
  // filing detials for first block
  first = (block_header *)heap;
  first->size_of_block = HEAP_SIZE;
  first->allocation_status = false;
  first->previous_block_status = true;
  heap_initiated = 1;

  // filing details for last block
  epilogue_block = first;

  // epilogue_block += (sizeof(heap) - sizeof(block_header)); this is incorrect and causes segmentation fault as the sizeof(block_header)
  // also includes the padding and it actually evaluates to - epilogue_block += ((sizeof(heap) - sizeof(block_header))) * sizeof(block_header)

  epilogue_block = (block_header *)((char *)heap + sizeof(heap) - sizeof(block_header)); // typecast it to block_header to declare the resultant location is a struct
  epilogue_block->size_of_block = 0;
  epilogue_block->allocation_status = true;
};

void *allocate_heap_memory(size_t size) {
  int padding = 0;
  if (!heap_initiated) {
    initiate_heap();
  };
  block_header *iterator = first; // one object two pointers so iterator->size is same as first->size
                                  // first fit algorithm
  void *mover = iterator;
  while (!(!iterator->allocation_status && (iterator->size_of_block - sizeof(block_header) >= size))) {
    if (iterator->size_of_block == 0 && iterator->allocation_status == true) {
      return NULL; // it's upon the user to check befoure using a returned pointer
    }
    mover = (char *)iterator + iterator->size_of_block;
    previous_block_ptr = iterator;
    previous_block_ptr->allocation_status = true;
    iterator = mover;
  }
  block_header *block_head = iterator;
  block_head->allocation_status = true;
  if (previous_block_ptr && previous_block_ptr->allocation_status) {
    block_head->previous_block_status = true;
  } else {
    block_head->previous_block_status = false;
  }

  void *payload_address = ((char *)block_head + 1);
  if ((uintptr_t)(payload_address) % 8) {
    padding = 8 - ((uintptr_t)(payload_address) % 8);
  }

  // size_of_block = header + padding + sizeof payload
  block_head->size_of_block = sizeof(block_header) + padding + size;

  int left_space = iterator->size_of_block - block_head->size_of_block;

  if (left_space >= (sizeof(block_header) + 8)) {
    block_header *block_head2 = iterator + block_head->size_of_block;
    block_head2->size_of_block = left_space;
  }

  void *return_ptr = (char *)block_head + sizeof(block_header) + padding;
  // padding to add in case an array is alloated

  return return_ptr;
};
