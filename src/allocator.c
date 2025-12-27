#include "../include/allocator.h"
#include <stdbool.h>
#include <stdint.h>

#define MIN_PAYLOAD_SIZE (sizeof(free_node))
#define MIN_BLOCK_SIZE (sizeof(block_header) + sizeof(free_node) + ALIGNMENT)
#define ROUND_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

// c preprocessor directives exist only befoure compiling because afterwards they are replaced by their value hence in gdb their names are not recognized
// it is garunteed that the below char array will start from a address that is 16-byte aligned so making first etc makes sence
unsigned char heap[HEAP_SIZE];
bool heap_initiated = 0;

typedef struct block_header {
  size_t size_of_block;
  bool allocation_status;
  bool previous_block_status;
  uint8_t _pad[ALIGNMENT - (sizeof(size_t) + 2) % ALIGNMENT];
} block_header;

typedef struct free_node {
  struct free_node *prev;
  struct free_node *next;

} free_node;

block_header *first;
block_header *epilogue_block;

block_header *previous_block_ptr;

void initiate_heap() {
  // free nodes need not be assigned to first and last block as they are just "boundary markers"
  first = (block_header *)heap;
  first->size_of_block = HEAP_SIZE;
  first->allocation_status = false;
  first->previous_block_status = true;
  heap_initiated = 1;

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

  // ITERATE TILL YOU REACH AN EMPTY BLOCK
  block_header *iterator = first; // one object two pointers so iterator->size is same as first->size
                                  // first fit algorithm
  void *mover = iterator;
  while (!(!iterator->allocation_status && (iterator->size_of_block - sizeof(block_header) >= size))) {
    // IF LAST BLOCK REACHED
    if (iterator->size_of_block == 0 && iterator->allocation_status == true) {
      return NULL; // it's upon the user to check befoure using a returned pointer
    }
    // ELSE KEEP MOVING
    mover = (char *)iterator + iterator->size_of_block;
    previous_block_ptr = iterator;
    iterator = mover;
  }
  // in this case this allocator does not implement a "prologue/sentinal block" to act as a boundary

  // MARK THE FOUND FREE BLOCK'S HEADER
  block_header *block_head = iterator;
  block_head->allocation_status = true;
  if (previous_block_ptr && previous_block_ptr->allocation_status) {
    block_head->previous_block_status = true;
  } else {
    block_head->previous_block_status = false;
  }

  // LEGACY CODE
  // void *payload_address = ((char *)block_head + 1);
  // void *payload_address = (char *)block_head + sizeof(block_header);
  //
  // if ((uintptr_t)(payload_address) % 8) {
  //   padding = 8 - ((uintptr_t)(payload_address) % 8);
  // }

  // MARK THE FOUND FREE BLOCK'S HEADER
  // size_of_block = header + padding + sizeof payload
  // block_head->size_of_block = sizeof(block_header) + padding + size;

  // ASSINING BLOCK TO LEFT OVER SPACE
  // int left_space = iterator->size_of_block - block_head->size_of_block;
  // LEGACY CODE

  size_t old_size = iterator->size_of_block;

  // block_head->size_of_block = sizeof(block_header) + ALIGNMENT + size;
  size_t raw_size = sizeof(block_header) + size;
  size_t aligned_size = ROUND_UP(raw_size, ALIGNMENT);
  block_head->size_of_block = aligned_size;

  // CALCULATING PADDING
  // int next_multiple_16 = ROUND_UP(block_head->size_of_block, 16);
  // padding = next_multiple_16 - block_head->size_of_block;
  //
  // block_head->size_of_block += padding;

  // int left_space = old_size - block_head->size_of_block;

  size_t left_space = old_size - block_head->size_of_block;

  if (left_space >= sizeof(block_header) + MIN_PAYLOAD_SIZE) {
    block_header *block_head2 = (block_header *)((char *)block_head + block_head->size_of_block);

    block_head2->size_of_block = left_space;
    block_head2->allocation_status = false;
    block_head2->previous_block_status = true;
    free_node *node = (free_node *)((char *)block_head2 + sizeof(block_header));
    node->prev = NULL;
    node->next = NULL;
  }
  // SPLITING THE HUGE FREE BLOCK INTO TWO
  // if (left_space >= block_head->size_of_block) {
  //   block_header *block_head2 = (block_header *)((char *)iterator + block_head->size_of_block);
  //   block_head2->size_of_block = left_space;
  //   block_head2->previous_block_status = true;
  //   block_head2->allocation_status = false;
  // }
  //
  // FINALLY RETURNING
  // void *return_ptr = (char *)block_head + sizeof(block_header) + MIN_PAYLOAD_SIZE;
  // padding to add in case an array is alloated

  // return return_ptr;
  void *return_ptr = (char *)block_head + sizeof(block_header);
  return return_ptr;
};

// void free(void *ptr) {
//
//   // MOVE POINTER TO HEADER TO MARK THE BLOCK FREE
//   block_header *header = (block_header *)((char *)ptr - sizeof(block_header)); // here how to determine how many bytes to move back when i don't know the padding ?
//   header->allocation_status = false;
//   block_header *checkpoint = header;
//
//   // COLLACING
//
//   // CHECK IF NEXT BLOCK IS FREE OR NOT AND COLACE IT IF FREE
//   header = (block_header *)((char *)header + header->size_of_block);
//   if (!header->allocation_status) {
//     int size_of_next_block = header->size_of_block;
//     header = checkpoint;
//     header->size_of_block = header->size_of_block + size_of_next_block;
//   }
//
//   // CHECK IF PREVIOUS BLOCK IS FREE OR NOT AND COLACE IT IF FREE
//   header = (block_header *)((char *)header + header->size_of_block);
// }

void free(void *ptr) {
  if (ptr == NULL)
    return;

  block_header *header = (block_header *)((char *)ptr - sizeof(block_header));

  header->allocation_status = false;

  free_node *node = (free_node *)((char *)header + sizeof(block_header));
  node->prev = NULL;
  node->next = NULL;

  block_header *next = (block_header *)((char *)header + header->size_of_block);

  if (next->size_of_block != 0 && !next->allocation_status) {
    header->size_of_block += next->size_of_block;
  }

  if (!header->previous_block_status) {
    block_header *iter = first;
    block_header *prev = NULL;

    while (iter != header && iter->size_of_block != 0) {
      prev = iter;
      iter = (block_header *)((char *)iter + iter->size_of_block);
    }

    if (prev && !prev->allocation_status) {
      prev->size_of_block += header->size_of_block;
      header = prev;
    }
  }

  block_header *after = (block_header *)((char *)header + header->size_of_block);

  if (after->size_of_block != 0) {
    after->previous_block_status = false;
  }
}
