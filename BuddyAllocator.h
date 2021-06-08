#pragma once
#include "pool_allocator.h"
#include "Bitmap.h" 
#include "Buddy.h"
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#define MAX_LEVELS 16
#define DATA_MAX uint16_t

typedef struct  BuddyAllocator{
    DATA_MAX num_levels;
    size_t buffer_size;
    DATA_MAX min_bucket_size;
    BitMap* bitmap;
    PoolAllocator *p_alloc;
    uint8_t* memory;
} BuddyAllocator;


int BuddyAllocator_calcSize(int num_levels);
void BuddyAllocator_init(
                    BitMap* b,
                    BuddyAllocator* alloc,
                    DATA_MAX num_levels,
                    uint8_t* buffer,
                    size_t buffer_size,
                    DATA_MAX min_bucket_size);

Buddy_item* BuddyAllocator_createBuddy(BuddyAllocator* alloc, int idx);
Buddy_item* BuddyAllocator_getBuddy(BuddyAllocator* alloc, DATA_MAX level);
void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, DATA_MAX idx);
void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size);
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);


