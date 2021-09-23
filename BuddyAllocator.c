#include "BuddyAllocator.h"
#define DEBUG 1

void BuddyAllocator_init(
    BitMap_tree* tree,
    BuddyAllocator* b_alloc,
    uint8_t* bm_buffer,
    uint8_t* memory,
    DATA_MAX buffer_size,
    DATA_MAX num_levels     ){

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

    b_alloc->min_bucket_size = min_bucket_size;

    const DATA_MAX leafs = tree_leafs(num_levels);
    const DATA_MAX user_mem_required = min_bucket_size * leafs;
    const DATA_MAX total_mem_required = internal_mem_required + user_mem;
    
    // the buffer for the list starts where the bitmap ends
    uint8_t *list_start= memory;
    PoolAllocatorResult init_result = PoolAllocator_init(
        b_alloc->p_alloc, 
        min_bucket_size,
        total_nodes, 
        list_start, 
        list_alloc_size
    );

    if (DEBUG){
        FILE *f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(f, "Initialized buffer...\n");
        fprintf(f, "Item size: %d\n", b_alloc->p_alloc->item_size);
        fprintf(f, "Items: %d\n", b_alloc->p_alloc->size);
        fprintf(f, "Mem Block addr: %p\n", b_alloc->p_alloc->buffer);
        fprintf(f, "Free list addr: %p\n", b_alloc->p_alloc->free_list);
        fprintf(f, "Buffer size: %d\n", b_alloc->p_alloc->buffer_size);
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fclose(f);
    }

    // we need room also for level 0
    b_alloc->num_levels=num_levels;
    b_alloc->memory=memory;
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

    if(DEBUG){
        FILE* f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f, "Requested Buddy at level %d\n", level);
        fclose(f);   
    }

    DATA_MAX offset = 0;
    assert(level>=0);
    assert(level<=b_alloc->num_levels);

    if(level<0)
        return 0;

    //Checks if there are buddies on the level, if not calls the same function on previous level
    if(!tree_free_buddies_on_level(b_alloc->tree, level)){
        if(level == 0){
            return NULL;
        }
        return BuddyAllocator_getBuddy(b_alloc, level-1);
    }

    if(tree_free_buddies_on_level(b_alloc->tree, level)){   
        if(level == 0){
            BitMap_setBit(b_alloc->tree->BitMap, 1, ALLOCATED);
            return (void*) b_alloc->memory + 1;
        }
        DATA_MAX first_idx = tree_first_free_node_level(b_alloc->tree, level);
        if(first_idx==0) 
            first_idx = tree_first_node_level(b_alloc->tree, level+1);
        if(!(first_idx-(tree_first_node_level(b_alloc->tree, first_idx)))){
            offset = b_alloc->min_bucket_size * (b_alloc->num_levels-level);
        } 
        else {
            DATA_MAX idx=tree_first_node_level(b_alloc->tree, first_idx);
            offset = b_alloc->min_bucket_size * (b_alloc->num_levels-level) * (first_idx-idx);
        }
        BitMap_setBit(b_alloc->tree->BitMap, first_idx, ALLOCATED);
        
        if(DEBUG){
            printf("Buddies on level %d: %d\n", level, tree_buddiesOnLevel(b_alloc->tree, level));
            printf("%d * (%d - %d) * (%d - %d)\n", b_alloc->min_bucket_size, b_alloc->num_levels, level, first_idx, tree_first_node_level(b_alloc->tree, first_idx));
            printf("[Address]:%p \t[Offset]: %d,\n", b_alloc->memory+offset, offset);
            tree_print(b_alloc->tree, F_CONCAT);
        } 

        return (void*) b_alloc->memory + offset;

    }

    return(0);
}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, void* item){
   DATA_MAX offset = -((void*)alloc->memory - item);
   printf("[Offset (Bytes)]: %d\t", offset);
   //printf("[Test]: %d\t", offset/alloc->min_bucket_size);
   //DATA_MAX item_size = offset/alloc->min_bucket_size*(alloc->num_levels);
   //printf("[Offset (Idx)]: %d\n", item_size);
}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size){
    //DATA_MAX mem_size = (1<<alloc->num_levels)*alloc->min_bucket_size;
    //Leave space (size of pointer) to write addr of target mem
    DATA_MAX level = tree_level(alloc->tree, alloc->user_mem/(size+sizeof(void*)));
    if(DEBUG)printf("[LEVEL]:%d\n",level);
    if(level>alloc->num_levels) level = alloc->num_levels-1;
    
    if (DEBUG){
        FILE* f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f, "Requested Buddy of %d bytes, level %d\n",size, level);
        fclose(f);   
    }
    
    void* buddy_ptr = BuddyAllocator_getBuddy(alloc, level);

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
        fprintf(f,"BuddyAllocator_initSingleBuffer|--------------------------------------------------------------\n");
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

    /*
    BitMap_tree* tree,
    BuddyAllocator* b_alloc,
    uint8_t* bm_buffer,
    uint8_t* memory,
    DATA_MAX buffer_size,
    DATA_MAX num_levels
    */
    


}