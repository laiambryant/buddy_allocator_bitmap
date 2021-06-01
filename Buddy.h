#pragma once
#include "BuddyAllocator.h"
#include "pool_allocator.h"
#include <stdint.h>

#define DATA_MAX uint16_t

typedef struct BuddyListItem {

} BuddyListItem;

//
inline DATA_MAX level(DATA_MAX tree_node);
//
inline DATA_MAX buddy(DATA_MAX tree_node);
//
inline DATA_MAX parent(DATA_MAX tree_node);

// creates list items using pool allocator inside buddy
BuddyListItem* BuddyListItem_alloc(BuddyAllocator* allocator);
//erases list items using pool allocator inside buddy
void BuddyListItem_free(BuddyAllocator* allocator, BuddyListItem* item);
//
BuddyListItem* BuddyAllocator_get(BuddyAllocator* allocator, DATA_MAX level);