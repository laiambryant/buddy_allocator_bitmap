#include "BuddyAllocator.h"
#define DEBUG 1

void BuddyAllocator_init(
    BitMap_tree* b,
    BuddyAllocator* b_alloc,
    uint8_t* memory,
    DATA_MAX num_levels,
    DATA_MAX buffer_size,
    DATA_MAX min_bucket_size){
        
    //init 
    b_alloc->tree=b;
    b_alloc->num_levels=num_levels;
    b_alloc->min_bucket_size=(buffer_size*8)/pow(2, MAX_LEVELS);
    b_alloc->buffer_size=buffer_size;
    b_alloc->memory=memory;
    assert(num_levels<=MAX_LEVELS);
    assert(buffer_size>=BuddyAllocator_calcSize(b_alloc->num_levels));
    
    b_alloc->num_items=(1<<(num_levels+1));
    size_t list_alloc_size = (sizeof(BitMap_tree)+sizeof(DATA_MAX))*b_alloc->num_items;
    uint8_t* list_start = memory;

    if (DEBUG) BuddyAllocator_printMetadata(b_alloc, F_WRITE);
    
    //Create Pool allocator
	PoolAllocator_init(
        b_alloc->p_alloc, 
        sizeof(Buddy_item), 
        b_alloc->num_items, 
        list_start, 
        list_alloc_size);

    BuddyAllocator_malloc(b_alloc, 1);

}


DATA_MAX BuddyAllocator_calcSize(DATA_MAX num_levels){
    DATA_MAX num_items=(1<<(num_levels+1));
    DATA_MAX list_alloc_size = (sizeof(Buddy_item)+sizeof(DATA_MAX))*num_items;
    return list_alloc_size;
}

Buddy_item* BuddyAllocator_getBuddy(BuddyAllocator* b_alloc, DATA_MAX idx){
    Buddy_item* item = (Buddy_item*)PoolAllocator_getBlock(b_alloc->p_alloc);
    item->idx=idx;
    BitMap_setBit(item->BitMap, idx, ALLOCATED); //Change value of index in bitmap

}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, DATA_MAX idx){

}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size){
    
}

void BuddyAllocator_free(BuddyAllocator* alloc, void* mem){

}

void BuddyAllocator_printMetadata(BuddyAllocator* b_alloc, OUT_MODE out){
    if(out==STDOUT){
        fprintf(stdout ,"\tBitmap Address: %p\n", b_alloc->tree->BitMap);
        fprintf(stdout ,"\tBuddy Allocator levels: %d\n", b_alloc->num_levels);
        fprintf(stdout ,"\tBuddy Allocator items: %d\n", b_alloc->num_items);
        fprintf(stdout ,"\tBuddy Allocator min bucket size: %d\n", b_alloc->min_bucket_size);
        fprintf(stdout ,"\tMem alloc size: %d\n", b_alloc->buffer_size);
        fprintf(stdout ,"\tMem address start: %p\n", b_alloc->memory);
        fprintf(stdout ,"\tMem address end: %p\n", (b_alloc->memory)+b_alloc->buffer_size);
        Bitmap_print(b_alloc->tree->BitMap, out);
    }
    if(out==F_CONCAT){
        FILE* f = fopen("OUT/allocator_metadata.txt", "a");
        fprintf(f,"\tBitmap Address: %p\n", b_alloc->tree->BitMap);
        fprintf(f,"\tBuddy Allocator levels: %d\n", b_alloc->num_levels);
        fprintf(f,"\tBuddy Allocator items: %d\n", b_alloc->num_items);
        fprintf(f,"\tBuddy Allocator min bucket size: %d\n", b_alloc->min_bucket_size);
        fprintf(f,"\tMem alloc size: %d\n", b_alloc->buffer_size);
        fprintf(f,"\tMem address start: %p\n", b_alloc->memory);
        fprintf(f,"\tMem address end: %p\n", (b_alloc->memory)+b_alloc->buffer_size);
        Bitmap_print(b_alloc->tree->BitMap, out);
        fclose(f);
    }
    if(out==F_WRITE){
        FILE* f = fopen("OUT/allocator_metadata.txt", "w");
        fprintf(f,"\tBitmap Address: %p\n", b_alloc->tree->BitMap);
        fprintf(f,"\tBuddy Allocator levels: %d\n", b_alloc->num_levels);
        fprintf(f,"\tBuddy Allocator items: %d\n", b_alloc->num_items);
        fprintf(f,"\tBuddy Allocator min bucket size: %d\n", b_alloc->min_bucket_size);
        fprintf(f,"\tMem alloc size: %d\n", b_alloc->buffer_size);
        fprintf(f,"\tMem address start: %p\n", b_alloc->memory);
        fprintf(f,"\tMem address end: %p\n", (b_alloc->memory)+b_alloc->buffer_size);
        Bitmap_print(b_alloc->tree->BitMap, F_CONCAT);
        fclose(f);
    }
}
