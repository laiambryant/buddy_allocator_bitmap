#pragma once
#include "pool_allocator.h"
#include "Bitmap_tree.h" 
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#define MAX_LEVELS 16
#define DATA_MAX uint32_t

typedef struct  BuddyAllocator{
    DATA_MAX num_levels;
    DATA_MAX buffer_size;
    DATA_MAX min_bucket_size;
    DATA_MAX num_items;
    BitMap_tree* tree;
    PoolAllocator *p_alloc;
    uint8_t* memory;
} BuddyAllocator;


uint32_t BuddyAllocator_calcSize(uint32_t num_levels);
void BuddyAllocator_init(
                    BitMap_tree* b,
                    BuddyAllocator* b_alloc,
                    uint8_t* buffer,
                    DATA_MAX num_levels,
                    DATA_MAX buffer_size,
                    DATA_MAX min_bucket_size);

Buddy_item* BuddyAllocator_getBuddy(BuddyAllocator* alloc, DATA_MAX idx);
void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, DATA_MAX idx);
void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size);
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);
void BuddyAllocator_printMetadata(BuddyAllocator* alloc, OUT_MODE out);


