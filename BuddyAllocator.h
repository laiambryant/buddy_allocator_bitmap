#pragma once
#include "pool_allocator.h"
#include "Bitmap.h" 
#include <stdint.h>
#include <math.h>
#include <stdint.h>
#include <assert.h>

#define MAX_LEVELS 16
#define DATA_MAX uint16_t


// one entry of the buddy list
typedef struct BuddyListItem {
    DATA_MAX idx;
    
} BuddyListItem;


typedef struct  BuddyAllocator{
    BitMap* bitmap;
    DATA_MAX num_levels;
    uint8_t* buffer;
    DATA_MAX buffer_size;
    uint8_t* memory;
    DATA_MAX min_bucket_size;
} BuddyAllocator;

DATA_MAX level(DATA_MAX tree_node);
DATA_MAX first_node_level(DATA_MAX tree_node);
DATA_MAX node_level_offset(DATA_MAX tree_node);
DATA_MAX buddy(DATA_MAX tree_node);
DATA_MAX parent(DATA_MAX tree_node);

DATA_MAX BuddyAllocator_calcSize(DATA_MAX num_levels);

void BuddyAllocator_init(
                        BitMap* bitmap,
                        BuddyAllocator* alloc,
                        DATA_MAX num_levels,
                        uint8_t* buffer,
                        DATA_MAX buffer_size,
                        uint8_t* memory,
                        DATA_MAX min_bucket_size);

BuddyListItem* BuddyAllocator_getBuddy(BuddyAllocator* alloc, DATA_MAX level);

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, BuddyListItem* item);

void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size);

void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);


