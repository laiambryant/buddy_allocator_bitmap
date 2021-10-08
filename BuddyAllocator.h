#pragma once
#include "pool_allocator.h"
#include "Bitmap_tree.h" 
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define MAX_LEVELS 16
#define DATA_MAX int

typedef struct  BuddyAllocator{
    DATA_MAX num_levels;
    DATA_MAX buffer_size;
    DATA_MAX user_mem;
    DATA_MAX min_bucket_size;
    DATA_MAX num_items;
    BitMap_tree* tree;
    PoolAllocator *p_alloc;
    uint8_t* memory;
} BuddyAllocator;

typedef struct BuddyItem{
    DATA_MAX idx;
    uint8_t* mem;
} Buddy_item;

static const char* BuddyAllocator_strerrors[]=
  {"Success",
   "NotEnoughMemory",
   "UnalignedFree",
   "OutOfRange",
   "DoubleFree",
   0
};

typedef enum {
  BA_Success=0x0,
  BA_NotEnoughMemory=1,
  BA_UnalignedFree=2,
  BA_OutOfRange=3,
  BA_DoubleFree=4
} BuddyAllocatorResult;

const char* BuddyAllocator_strerror(PoolAllocatorResult result);

DATA_MAX BuddyAllocator_calcSize(DATA_MAX num_levels);
void BuddyAllocator_init(
                    BitMap_tree* tree,
                    BuddyAllocator* b_alloc,
                    uint8_t* bm_buffer,
                    uint8_t* memory,
                    DATA_MAX buffer_size,
                    DATA_MAX num_levels         
                    );
void BuddyAllocator_initSingleBuffer(
    BuddyAllocator* b_alloc,
    PoolAllocator* p_alloc, 
    uint8_t* allocator_mem,
    BitMap_tree* tree,
    DATA_MAX allocator_mem_size,
    DATA_MAX num_levels
    );
void* BuddyAllocator_getBuddy(BuddyAllocator* alloc, DATA_MAX level);
BuddyAllocatorResult BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, void* item);
void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size);
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);
void BuddyAllocator_printMetadata(BuddyAllocator* alloc, OUT_MODE out);