#pragma once
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
    DATA_MAX leaf_num;
    BitMap_tree* tree;
    uint8_t* memory;
} BuddyAllocator;

static const char* BuddyAllocator_strerrors[]={"Success",
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

//Gets string error corresponding to errnumber
const char* BuddyAllocator_strerror(BuddyAllocatorResult result);

//calculates the size of the BuddyAllocator
DATA_MAX BuddyAllocator_calcSize(DATA_MAX num_levels);
//Initializes Buddy allocator
BuddyAllocator* BuddyAllocator_init( 
  uint8_t* ba_buffer,
  DATA_MAX buffer_size,
  DATA_MAX num_levels         
);
//Given a level, returns the mem address corresponding to the buddy given by buddy allocator
void* BuddyAllocator_getBuddy(BuddyAllocator* alloc, DATA_MAX level);
//Releases item pointed by item* pointer, fetching idx from 4 bits preceding the address
BuddyAllocatorResult BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, void* item);
//Calculates level of buddy alloc to serve memory request and calls getBuddy. Returns pointer to memory zone
void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size);
//Calls releaseBuddy to free memory pointed by mem address 
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);
//Prints metadata for buddy allocator
void BuddyAllocator_printMetadata(BuddyAllocator* alloc, OUT_MODE out);