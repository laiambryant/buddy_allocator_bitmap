#include "BuddyAllocator.h"
#define DEBUG 1



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
    int list_alloc_size=(sizeof(Buddy_item)+sizeof(int))*list_items;

    const DATA_MAX total_nodes = pow(2, num_levels + 1) - 1;
    const DATA_MAX internal_mem_required = (sizeof(Buddy_item)+sizeof(int))* total_nodes;
    const DATA_MAX user_mem = buffer_size-internal_mem_required;
    const DATA_MAX min_bucket_size = (user_mem>>(num_levels));
    const DATA_MAX leafs = tree_leafs(num_levels);
    const DATA_MAX user_mem_required = min_bucket_size * leafs;
    
    // the buffer for the list starts where the bitmap ends
    uint8_t *list_start= memory;

    b_alloc->min_bucket_size = min_bucket_size;
    b_alloc->tree = tree;
    b_alloc->buffer_size = buffer_size;
    b_alloc->num_items = 1<<(num_levels+1);
    b_alloc->user_mem=user_mem_required;
    
    assert (num_levels<MAX_LEVELS);
    assert(buffer_size>=internal_mem_required); 

    BitMap_setBit(b_alloc->tree->BitMap, 0, ALLOCATED);

}

DATA_MAX BuddyAllocator_calcSize(DATA_MAX num_levels){
    DATA_MAX num_items=(1<<(num_levels+1))-1;
    DATA_MAX list_alloc_size = (sizeof(Buddy_item)+sizeof(DATA_MAX))*num_items;
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
        
        if(!(idx-(tree_first_node_level(b_alloc->tree, idx))))
            offset = b_alloc->min_bucket_size * (b_alloc->num_levels-level);
        else
            offset = b_alloc->min_bucket_size * (b_alloc->num_levels-level) * (idx-(tree_first_node_level(b_alloc->tree, idx)));

        void* ret = (void*) b_alloc->memory + offset;

        if(DEBUG){

            FILE* f = fopen("OUT/Logs/log.txt", "a");
            fprintf(f,"%d * (%d - %d) * (%d - %d) = %d\n", b_alloc->min_bucket_size, b_alloc->num_levels, level, idx, tree_first_node_level(b_alloc->tree, idx), offset);
            fprintf(f,"[Address]:%p \t[Offset]: %d,\n", ret, offset);
            fclose(f); 

            tree_print(b_alloc->tree, F_CONCAT);

        }
        
        return ret;
    }
        
    return(0);
}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, void* item){
   
}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size){
    
    //Leave space (size of pointer) to write addr of target mem
    DATA_MAX level = tree_level(alloc->tree, alloc->user_mem/(size+sizeof(void*)));
    if(level>alloc->num_levels) level = alloc->num_levels-1;
    
    if (DEBUG){
        FILE* f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f, "\nRequested Buddy of %d bytes, level %d\n",size, level);
        fclose(f);   
    }
    
    void* buddy_ptr = BuddyAllocator_getBuddy(alloc, level);

    //If getBuddy does not find buddy it will return 0x0 as an address so we will exit
    assert(buddy_ptr!=0x0);

    return buddy_ptr;
}

void BuddyAllocator_free(BuddyAllocator* alloc, void* mem){
    if(DEBUG){
        FILE* f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f, "Freeing memory @%p\n", mem);
        fclose(f);   
    }

    char* p = (char*) mem; p-=8;
    Buddy_item* buddy_ptr = (Buddy_item*)p;
    Buddy_item* buddy = buddy_ptr;
    BuddyAllocator_releaseBuddy(alloc, buddy);
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

//Initializes buddy allocator with one single buffer
void BuddyAllocator_initSingleBuffer(
        BuddyAllocator* b_alloc, 
        PoolAllocator* p_alloc,
        uint8_t* allocator_mem,
        BitMap_tree* tree,
        DATA_MAX allocator_mem_size,
        DATA_MAX num_levels){
    
    //Checks if allocator and memory buffer are correctly initialized and if memory buffer was
    assert(b_alloc);
    assert(allocator_mem_size!=0 && allocator_mem);

    b_alloc->p_alloc = p_alloc;

    const DATA_MAX total_nodes = pow(2, num_levels + 1) - 1;
    const DATA_MAX internal_mem_required = (sizeof(Buddy_item)+sizeof(int))* total_nodes;
    const DATA_MAX user_mem = allocator_mem_size-internal_mem_required;
    const DATA_MAX min_bucket_size = (user_mem>>(num_levels));
    const DATA_MAX leafs = tree_leafs(num_levels);
    const DATA_MAX user_mem_required = min_bucket_size * leafs;
    const DATA_MAX total_mem_required = internal_mem_required + user_mem;

    if(DEBUG){
        FILE *f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(f,"BuddyAllocator_initSingleBuffer|current configuration\n");
        fprintf(f," |- memory available ........ [ %d ]\n",allocator_mem_size);
        fprintf(f," |- tree levels ............. [ %02d ]\n",num_levels);
        fprintf(f," |- tree nodes .............. [ %d ]\n",total_nodes);
        fprintf(f," |- leaf number ............. [ %d ]\n",leafs);
        fprintf(f," |- minimum-bs .............. [ %d ]\n",min_bucket_size);
        fprintf(f," |- total internal memory ... [ %d ]\n",internal_mem_required);
        fprintf(f," |- total user memory ....... [ %d ]\n",user_mem_required);
        fprintf(f," |- total required memory ... [ %d ]\n",total_mem_required);
        fprintf(f,"BuddyAllocator_initSingleBuffer|--------------------------------------------------------------\n\n");
        if (total_mem_required > allocator_mem_size) {
            fprintf(f,"BuddyAllocator_initSingleBuffer|ERROR, invalid configuration\n");
            return;
        }
        fclose(f);
    }
    
    uint8_t *list_start = allocator_mem; 
    uint8_t *mem_start = allocator_mem+internal_mem_required;
    
    BuddyAllocator_init(
        tree,
        b_alloc,
        list_start,
        mem_start,
        total_mem_required,
        num_levels
    );

}

Buddy_item* BuddyAllocator_createItem(BuddyAllocator* alloc, DATA_MAX idx, Buddy_item* parent){
    
    Buddy_item *item = (Buddy_item*) PoolAllocator_getBlock(alloc->p_alloc);

    item->idx = idx;
    DATA_MAX delta_idx = (idx-(tree_first_node_level(alloc->tree, idx)));
    if(!delta_idx)
        item->mem = alloc->memory + 
            alloc->min_bucket_size * 
            (alloc->num_levels-tree_level(alloc->tree, idx));
    else
        item->mem = alloc->memory + 
                (alloc->min_bucket_size * 
                (alloc->num_levels-(tree_first_node_level(alloc->tree, idx))) 
                * delta_idx);
    
    return item;

}

void BuddyAllocator_destroyItem(BuddyAllocator* alloc, Buddy_item* item){

    DATA_MAX lvl = tree_level(alloc->tree, item->idx);

    PoolAllocatorResult res = PoolAllocator_releaseBlock(alloc->p_alloc, item);
    assert(res==Success);

}