#include "BuddyAllocator.h"
#define DEBUG 1

void BuddyAllocator_init(
    BitMap* b,
    BuddyAllocator* b_alloc,
    DATA_MAX num_levels,
    uint8_t* memory,
    size_t buffer_size,
    DATA_MAX min_bucket_size){
    
    if(DEBUG) printf("Initializing buddy allocator:\n");
    //init 
    b_alloc->bitmap=b;
    b_alloc->num_levels=num_levels;
    b_alloc->min_bucket_size=min_bucket_size;
    b_alloc->buffer_size=buffer_size;
    b_alloc->memory=memory;
    assert(num_levels<=MAX_LEVELS);
    assert(buffer_size>=BuddyAllocator_calcSize(b_alloc->num_levels));
    
    size_t num_items=(1<<(num_levels+1));
    size_t list_alloc_size = (sizeof(Buddy_item)+sizeof(DATA_MAX))*num_items;
    uint8_t* list_start = memory;

    if (DEBUG){
       printf("\tBitmap Address: %p\n", b_alloc->bitmap);
       printf("\tBuddy Allocator levels: %d\n", num_levels);
       printf("\tBuddy Allocator items: %ld\n", num_items);
       printf("\tBuddy Allocator min bucket size: %d\n", b_alloc->min_bucket_size);
       printf("\tMem alloc size: %ld\n", b_alloc->buffer_size);
       printf("\tMem address start: %p\n", b_alloc->memory);
       printf("\tMem address end: %p\n", (b_alloc->memory)+b_alloc->buffer_size);
    }

    //Create Pool allocator
	PoolAllocator_init(
        b_alloc->p_alloc, 
        sizeof(Buddy_item), 
        num_items, 
        list_start, 
        list_alloc_size);

    BuddyAllocator_createBuddy(b_alloc, 0);

}

Buddy_item* BuddyAllocator_createBuddy(BuddyAllocator* alloc, int idx){

}

int BuddyAllocator_calcSize(int num_levels){
    int num_items=(1<<(num_levels+1));
    int list_alloc_size = (sizeof(Buddy_item)+sizeof(int))*num_items;
    return list_alloc_size;
}

Buddy_item* BuddyAllocator_getBuddy(BuddyAllocator* alloc, DATA_MAX level){

}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, DATA_MAX idx){

}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size){

 
}

void BuddyAllocator_free(BuddyAllocator* alloc, void* mem){

}