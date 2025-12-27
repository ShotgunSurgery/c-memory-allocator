#include <stddef.h>

#pragma once

#define HEAP_SIZE 1000
#define ALIGNMENT 6
extern unsigned char heap[HEAP_SIZE];

struct block_header;
struct free_list_block_header;

void initiate_heap();
void *allocate_heap_memory(size_t size);
void free(void *ptr);
