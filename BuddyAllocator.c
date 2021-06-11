#include "BuddyAllocator.h"
#define DEBUG 1

PoolAllocator p_alloc;


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
    b_alloc->p_alloc=&p_alloc;
    assert(num_levels<=MAX_LEVELS);
    assert(buffer_size>=BuddyAllocator_calcSize(b_alloc->num_levels));
    
    b_alloc->num_items=(1<<(num_levels+1));
    size_t list_alloc_size = (sizeof(BitMap_tree)+sizeof(DATA_MAX))*b_alloc->num_items;
    uint8_t* list_start = memory;

    if (DEBUG) BuddyAllocator_printMetadata(b_alloc, F_WRITE);
    
    DATA_MAX num_items_max = 1<<(num_levels+1);
    PoolAllocatorResult res = PoolAllocator_init(
        &p_alloc, min_bucket_size, num_items_max, memory, buffer_size
    );
    assert(res==Success);

    Buddy_item* item0 =BuddyAllocator_createItem(b_alloc, 1);

    if(DEBUG){
        FILE* f = fopen("OUT/log.txt", "a");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(f,"Creating First item of Buddy allocator...\n");
        fprintf(f,"Item Metadata:\tBitmap Addr:\t%p,\t\tMemory start addr:\t%p\n", item0->BitMap, item0->memory);
        fprintf(f,"              \tItem idx:   \t%d,\t\tItem lvl:         \t%d\n", item0->idx, item0->level);
        fprintf(f,"              \tParent idx: \t%d,\t\tBuddy idx:        \t%d\n", item0->parent_idx, item0->buddy_idx);
        fprintf(f,"              \tItem size:  \t%d\n", item0->size);
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fclose(f);
    }

}


DATA_MAX BuddyAllocator_calcSize(DATA_MAX num_levels){
    DATA_MAX num_items=(1<<(num_levels+1));
    DATA_MAX list_alloc_size = (sizeof(Buddy_item)+sizeof(DATA_MAX))*num_items;
    return list_alloc_size;
}

Buddy_item* BuddyAllocator_createItem(BuddyAllocator* b_alloc, DATA_MAX idx){
    Buddy_item* item = (Buddy_item*)PoolAllocator_getBlock(b_alloc->p_alloc);
    item->idx=idx;
    item->level = tree_level(idx);
    item->memory=b_alloc->memory+(idx-(1<<tree_level(idx)))*b_alloc->min_bucket_size;
    item->size=(1<<(b_alloc->num_items-item->level))*b_alloc->min_bucket_size;
    item->parent_idx = tree_getparent(idx);
    item->buddy_idx = tree_getbuddy(idx);
    item->BitMap=b_alloc->tree;
    return item;
}

void BuddyAllocator_destroyItem(BuddyAllocator* b_alloc, Buddy_item* item){
    DATA_MAX level = item->level;
    PoolAllocatorResult res = PoolAllocator_releaseBlock(b_alloc->p_alloc, item);
    assert(res==Success);
}

Buddy_item* BuddyAllocator_getBuddy(BuddyAllocator* b_alloc, DATA_MAX level){
    if(level<0) return 0;
    assert(level<=b_alloc->num_levels);
    if(!tree_buddiesOnLevel(b_alloc->tree, level)){
        Buddy_item* parent = BuddyAllocator_getBuddy(b_alloc, level-1);
        if(!parent) return 0;
    }
    
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
