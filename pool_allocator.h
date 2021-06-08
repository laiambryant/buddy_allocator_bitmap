#pragma once
#include <stdint.h>
#include <stdio.h>

typedef enum {
  Success=0x0,
  NotEnoughMemory=1,
  UnalignedFree=2,
  OutOfRange=3,
  DoubleFree=4
} PoolAllocatorResult;

typedef struct PoolAllocator{
  
  uint8_t* buffer;        //contiguous buffer managed by the system
  int*  free_list;     //list of linked objects
  int buffer_size;     //size of the buffer in bytes

  int size;            //number of free blocks
  int size_max;        //maximum number of blocks
  int item_size;       //size of a block
  
  int first_idx;       //pointer to the first bucket
  int bucket_size;     // size of a bucket
} PoolAllocator;

PoolAllocatorResult PoolAllocator_init(PoolAllocator* allocator,
			size_t item_size,
			int num_items,
			uint8_t* memory_block,
			size_t memory_size);

void* PoolAllocator_getBlock(PoolAllocator* allocator);

PoolAllocatorResult PoolAllocator_releaseBlock(PoolAllocator* allocator, void* block);
			
const char* PoolAllocator_strerror(PoolAllocatorResult result);
