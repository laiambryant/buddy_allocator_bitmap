#include "BuddyAllocator.h"
#define DEBUG 1


const char* BuddyAllocator_strerror(PoolAllocatorResult result) {
  return BuddyAllocator_strerrors[result];
}

void BuddyAllocator_init(
    BitMap_tree* tree,
    BuddyAllocator* b_alloc,
    uint8_t* bm_buffer,
    uint8_t* memory,
    DATA_MAX buffer_size,
    DATA_MAX num_levels  ){

    // we need enough memory to handle internal structures
    assert (buffer_size>=BuddyAllocator_calcSize(num_levels));

    b_alloc->num_levels = num_levels;
    b_alloc->memory = memory;

    int list_items=1<<(num_levels+1); // maximum number of allocations, used to size the list
    int list_alloc_size=list_items;

    const DATA_MAX min_bucket_size = (buffer_size>>(num_levels));
    
    // the buffer for the list starts where the bitmap ends
    uint8_t *list_start= memory;

    b_alloc->min_bucket_size = min_bucket_size;
    b_alloc->tree = tree;
    b_alloc->buffer_size = buffer_size;
    b_alloc->num_items = 1<<(num_levels+1);
    b_alloc->user_mem=buffer_size;
    
    assert (num_levels<MAX_LEVELS);

}

DATA_MAX BuddyAllocator_calcSize(DATA_MAX num_levels){
    DATA_MAX num_items=(1<<(num_levels+1))-1;
    DATA_MAX list_alloc_size = num_items;
    return list_alloc_size;
}

void* BuddyAllocator_getBuddy(BuddyAllocator* b_alloc, DATA_MAX level){

    DATA_MAX offset = 0;
    assert(level>=0);
    assert(level<=b_alloc->num_levels);

    if(level<0)
        return 0;

    //Checks if there are free buddies on the level, if not, returns 0x0
    if(!tree_free_buddies_on_level(b_alloc->tree, level))
        return 0;

    if(tree_free_buddies_on_level(b_alloc->tree, level)){
        /*
        Gets first free index for that level (free idx is assumed to exist because we already checked
        if there are free buddies on that level).
        */
        DATA_MAX idx = tree_balloc_getIdx(b_alloc->tree, level);
        if(idx==0) return 0;

        //Calculates offset
        offset = b_alloc->min_bucket_size * (b_alloc->num_levels-level) * (idx-(tree_first_node_level(b_alloc->tree, idx)));
        void* ret = (void*) b_alloc->memory + offset;
        
        //Write idx in first bytes of memory and returns memory address + sizeof(datatype)
        DATA_MAX* tmp = ret;
        *tmp = idx;
        ret = ret+sizeof(DATA_MAX);

        if(DEBUG){

            FILE* f = fopen("OUT/Logs/log.txt", "a");
            fprintf(f,"%d * (%d - %d) * (%d - %d) = %d\n", b_alloc->min_bucket_size, b_alloc->num_levels, level, idx, tree_first_node_level(b_alloc->tree, idx), offset);
            fprintf(f,"[Address]:%p \t[Offset]: %d, [Idx]: %d \n\n", ret, offset, *tmp);
            fclose(f); 

            tree_print(b_alloc->tree, F_CONCAT);

        }

        return ret;
    }
        
    return(0);
}

BuddyAllocatorResult BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, void* item){

    //Fetches idx pointer from the address (subtracting the size of the the datatype chosen)
    DATA_MAX* idx_ptr = (DATA_MAX*)(item-sizeof(DATA_MAX));
    DATA_MAX idx = *idx_ptr; 

    DATA_MAX offset = ((uint8_t*)item-sizeof(DATA_MAX))-alloc->memory;

    if(DEBUG){
        FILE* f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f, "Freeing memory: @%p\t", idx_ptr);
        fprintf(f,"Item idx: [%d], num_items[%d], offset[%d]\n",  idx, alloc->num_items, offset);
        fclose(f);   
    }

    if(tree_getBit(alloc->tree, idx)==FREE) 
        return BA_DoubleFree;

    if (offset%alloc->min_bucket_size)
        return BA_UnalignedFree;
    
    if (idx<0 || idx>=alloc->num_items)
        return BA_OutOfRange;

    tree_setBit(alloc->tree, idx, FREE);
    //Parents will be freed only if buddy of the node is also free 
    tree_setParents(alloc->tree, idx, tree_level(alloc->tree, idx), FREE);
    tree_setChildren(alloc->tree, idx, FREE);
    if(DEBUG) 
        tree_print(alloc->tree, F_CONCAT);

    return Success;    
}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size){
    
    //Leave space (size of pointer) to write addr of target mem
    DATA_MAX level = tree_level(alloc->tree, alloc->user_mem/(size+sizeof(DATA_MAX)));
    if(level>alloc->num_levels) level = alloc->num_levels-1;
    
    if (DEBUG){
        FILE* f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f, "Requested Buddy of %d bytes, level %d\n",size, level);
        fclose(f);   
    }
    
    void* buddy_ptr = BuddyAllocator_getBuddy(alloc, level);
    //If getBuddy does not find buddy it will return 0x0 as an address so we will exit
    if(buddy_ptr==0x0){
        printf("Malloc: %s\n", BuddyAllocator_strerror(BA_NotEnoughMemory));
        assert(buddy_ptr!=0x0);
    }

    return buddy_ptr;
}

void BuddyAllocator_free(BuddyAllocator* alloc, void* mem){
    BuddyAllocatorResult res = BuddyAllocator_releaseBuddy(alloc, mem);

    if(DEBUG){
        FILE* f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f, "Result: %s", BuddyAllocator_strerror(res));
        fclose(f);
    }

}

void BuddyAllocator_printMetadata(BuddyAllocator* b_alloc, OUT_MODE out){
    if(out==STDOUT){
        fprintf(stdout, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(stdout,"\tTree Address: %p\n", b_alloc->tree);
        fprintf(stdout ,"\tBitmap Address: %p\n", b_alloc->tree->BitMap);
        fprintf(stdout ,"\tBuddy Allocator levels: %d\n", b_alloc->num_levels);
        fprintf(stdout ,"\tBuddy Allocator items: %d\n", b_alloc->num_items);
        fprintf(stdout ,"\tBuddy Allocator min bucket size: %d\n", b_alloc->min_bucket_size);
        fprintf(stdout ,"\tMem alloc size: %d\n", b_alloc->buffer_size);
        fprintf(stdout ,"\tMem address start: %p\n", b_alloc->memory);
        fprintf(stdout ,"\tMem address end: %p\n", (b_alloc->memory)+b_alloc->buffer_size);
        Bitmap_print(b_alloc->tree->BitMap, out);
        fprintf(stdout, "\n----------------------------------------------------------------------------------------------\n");
    }
    if(out==F_CONCAT){
        FILE* f = fopen("OUT/Logs/allocator_metadata.txt", "a");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(f,"\tTree Address: %p\n", b_alloc->tree);
        fprintf(f,"\tBitmap Address: %p\n", b_alloc->tree->BitMap);
        fprintf(f,"\tBuddy Allocator levels: %d\n", b_alloc->num_levels);
        fprintf(f,"\tBuddy Allocator items: %d\n", b_alloc->num_items);
        fprintf(f,"\tBuddy Allocator min bucket size: %d\n", b_alloc->min_bucket_size);
        fprintf(f,"\tMem alloc size: %d\n", b_alloc->buffer_size);
        fprintf(f,"\tMem address start: %p\n", b_alloc->memory);
        fprintf(f,"\tMem address end: %p\n", (b_alloc->memory)+b_alloc->buffer_size);
        Bitmap_print(b_alloc->tree->BitMap, out);
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fclose(f);
    }
    if(out==F_WRITE){
        FILE* f = fopen("OUT/Logs/allocator_metadata.txt", "w");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(f,"\tTree Address: %p\n", b_alloc->tree);
        fprintf(f,"\tBitmap Address: %p\n", b_alloc->tree->BitMap);
        fprintf(f,"\tBuddy Allocator levels: %d\n", b_alloc->num_levels);
        fprintf(f,"\tBuddy Allocator items: %d\n", b_alloc->num_items);
        fprintf(f,"\tBuddy Allocator min bucket size: %d\n", b_alloc->min_bucket_size);
        fprintf(f,"\tMem alloc size: %d\n", b_alloc->buffer_size);
        fprintf(f,"\tMem address start: %p\n", b_alloc->memory);
        fprintf(f,"\tMem address end: %p\n", (b_alloc->memory)+b_alloc->buffer_size);
        Bitmap_print(b_alloc->tree->BitMap, F_CONCAT);
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fclose(f);
    }
}
