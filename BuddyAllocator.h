#pragma once
#include "Bitmap_tree.h"
#include <string.h>

#define MAX_LEVELS 16

typedef struct BuddyAllocator {
    DATA_MAX num_levels;
    DATA_MAX buffer_size;
    DATA_MAX user_mem;
    DATA_MAX min_bucket_size;
    DATA_MAX num_items;
    DATA_MAX leaf_num;
    BitMap_tree* tree;
    uint8_t* memory;
} BuddyAllocator;

typedef enum {
    BA_Success = 0x0,
    BA_NotEnoughMemory = 1,
    BA_UnalignedFree = 2,
    BA_OutOfRange = 3,
    BA_DoubleFree = 4
} BuddyAllocatorResult;

const char* BuddyAllocator_strerror(BuddyAllocatorResult result);
BuddyAllocator* BuddyAllocator_init(uint8_t* ba_buffer, DATA_MAX buffer_size, DATA_MAX num_levels);
void* BuddyAllocator_getBuddy(BuddyAllocator* alloc, DATA_MAX level);
BuddyAllocatorResult BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, void* item);
void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size);
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);
void BuddyAllocator_printMetadata(BuddyAllocator* alloc, FILE *out);
