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
    DATA_MAX min_bucket_size;
    DATA_MAX num_items;
    BitMap_tree* tree;
    PoolAllocator *p_alloc;
    uint8_t* memory;
} BuddyAllocator;

DATA_MAX BuddyAllocator_calcSize(DATA_MAX num_levels);
Buddy_item* BuddyAllocator_createItem(BuddyAllocator* b_alloc, DATA_MAX idx);
void BuddyAllocator_destroyItem(BuddyAllocator* b_alloc, Buddy_item* item);
void BuddyAllocator_init(
                    BitMap_tree* tree,
                    BuddyAllocator* b_alloc,
                    PoolAllocator* p_alloc,
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
Buddy_item* BuddyAllocator_getBuddy(BuddyAllocator* alloc, DATA_MAX level);
void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, Buddy_item* item);
void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size);
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);
void BuddyAllocator_printMetadata(BuddyAllocator* alloc, OUT_MODE out);

