#include "Buddy_allocator.h"

#define MAX_LEVELS 16

// one entry of the buddy list
typedef struct BuddyListItem {

} BuddyListItem;


typedef struct  {

} BuddyAllocator;


int BuddyAllocator_calcSize(int num_levels){

}


void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         char* buffer,
                         int buffer_size,
                         char* memory,
                         int min_bucket_size){

}

BuddyListItem* BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){

}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, BuddyListItem* item){

}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size){

}

void BuddyAllocator_free(BuddyAllocator* alloc, void* mem){

}