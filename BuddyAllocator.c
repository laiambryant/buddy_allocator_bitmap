#include "BuddyAllocator.h"
#define DEBUG 1

void BuddyAllocator_init(
    BitMap_tree* b,
    BuddyAllocator* b_alloc,
    DATA_MAX num_levels,
    uint8_t* memory,
    DATA_MAX buffer_size,
    DATA_MAX min_bucket_size){
    
    if(DEBUG) printf("Initializing buddy allocator:\n");
    
    //init 
    b_alloc->bitmap=b;
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

Buddy_item* BuddyAllocator_getBuddy(BuddyAllocator* alloc, DATA_MAX idx){
    if(idx>=0) return 0;
    assert(idx<=alloc->num_levels);
    if(!BitMap_bit(alloc->bitmap, idx)){
        if(!BitMap_bit(alloc->bitmap,parent(idx))){
            return 0;
        }
    }
}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, DATA_MAX idx){

}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size){
    
}

void BuddyAllocator_free(BuddyAllocator* alloc, void* mem){

}

void BuddyAllocator_printMetadata(BuddyAllocator* alloc, OUT_MODE out){
    if(out==STDOUT){
        fprintf(stdout ,"\tBitmap Address: %p\n", alloc->bitmap);
        fprintf(stdout ,"\tBuddy Allocator levels: %d\n", alloc->num_levels);
        fprintf(stdout ,"\tBuddy Allocator items: %ld\n", alloc->num_items);
        fprintf(stdout ,"\tBuddy Allocator min bucket size: %d\n", alloc->min_bucket_size);
        fprintf(stdout ,"\tMem alloc size: %ld\n", alloc->buffer_size);
        fprintf(stdout ,"\tMem address start: %p\n", alloc->memory);
        fprintf(stdout ,"\tMem address end: %p\n", (alloc->memory)+alloc->buffer_size);
        Bitmap_print(alloc->bitmap, out);
    }
    if(out==F_CONCAT){
        FILE* f = fopen("OUT/allocator_metadata.txt", "a");
        fprintf(f,"\tBitmap Address: %p\n", alloc->bitmap);
        fprintf(f,"\tBuddy Allocator levels: %d\n", alloc->num_levels);
        fprintf(f,"\tBuddy Allocator items: %ld\n", alloc->num_items);
        fprintf(f,"\tBuddy Allocator min bucket size: %d\n", alloc->min_bucket_size);
        fprintf(f,"\tMem alloc size: %ld\n", alloc->buffer_size);
        fprintf(f,"\tMem address start: %p\n", alloc->memory);
        fprintf(f,"\tMem address end: %p\n", (alloc->memory)+alloc->buffer_size);
        Bitmap_print(alloc->bitmap, out);
        fclose(f);
    }
    if(out==F_WRITE){
        FILE* f = fopen("OUT/allocator_metadata.txt", "w");
        fprintf(f,"\tBitmap Address: %p\n", alloc->bitmap);
        fprintf(f,"\tBuddy Allocator levels: %d\n", alloc->num_levels);
        fprintf(f,"\tBuddy Allocator items: %ld\n", alloc->num_items);
        fprintf(f,"\tBuddy Allocator min bucket size: %d\n", alloc->min_bucket_size);
        fprintf(f,"\tMem alloc size: %ld\n", alloc->buffer_size);
        fprintf(f,"\tMem address start: %p\n", alloc->memory);
        fprintf(f,"\tMem address end: %p\n", (alloc->memory)+alloc->buffer_size);
        Bitmap_print(alloc->bitmap, F_CONCAT);
        fclose(f);
    }
}
