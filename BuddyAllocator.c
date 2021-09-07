#include "BuddyAllocator.h"
#define DEBUG 1

void BuddyAllocator_init(
    BitMap_tree* tree,
    BuddyAllocator* b_alloc,
    PoolAllocator* p_alloc,
    uint8_t* bm_buffer,
    uint8_t* memory,
    DATA_MAX buffer_size,
    DATA_MAX num_levels     ){

    // we need enough memory to handle internal structures
    assert (buffer_size>=BuddyAllocator_calcSize(num_levels));

    int list_items=1<<(num_levels+1); // maximum number of allocations, used to size the list
    int list_alloc_size=(sizeof(Buddy_item)+sizeof(int))*list_items;

    const DATA_MAX total_nodes = pow(2, num_levels + 1) - 1;
    const DATA_MAX internal_mem_required = (sizeof(Buddy_item)+sizeof(int))* total_nodes;
    const DATA_MAX user_mem = buffer_size-internal_mem_required;
    const DATA_MAX min_bucket_size = (user_mem>>(num_levels));
    const DATA_MAX leafs = tree_leafs(num_levels);
    const DATA_MAX user_mem_required = min_bucket_size * leafs;
    const DATA_MAX total_mem_required = internal_mem_required + user_mem;

    if(DEBUG){
        FILE *f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(f,"BuddyAllocator_initSingleBuffer|current configuration\n");
        fprintf(f," |- memory available ....... [ %d ]\n",buffer_size);
        fprintf(f," |- tree levels ............. [ %02d ]\n",num_levels);
        fprintf(f," |- tree nodes .............. [ %d ]\n",total_nodes);
        fprintf(f," |- leaf number ............. [ %d ]\n",leafs);
        fprintf(f," |- minimum-bs .............. [ %d ]\n",min_bucket_size);
        fprintf(f," |- total internal memory ... [ %d ]\n",internal_mem_required);
        fprintf(f," |- total user memory ....... [ %d ]\n",user_mem_required);
        fprintf(f," |- total required memory ... [ %d ]\n",total_mem_required);
        fprintf(f,"BuddyAllocator_initSingleBuffer|---------------------\n");
        if (total_mem_required > buffer_size) {
            fprintf(f,"BuddyAllocator_initSingleBuffer|ERROR, invalid configuration\n");
            return;
        }
        fclose(f);
    }
  
    
    // the buffer for the list starts where the bitmap ends
    uint8_t *list_start= (uint8_t*)memory;
    PoolAllocatorResult init_result = PoolAllocator_init(
        p_alloc, 
        sizeof(Buddy_item),
        total_nodes, 
        list_start, 
        list_alloc_size
    );

    // we need room also for level 0
    b_alloc->num_levels=num_levels;
    b_alloc->memory=memory;
    b_alloc->tree = tree;
    b_alloc->min_bucket_size = min_bucket_size; 
    b_alloc->buffer_size = buffer_size;
    b_alloc->num_items = 1<<(num_levels+1);
    assert (num_levels<MAX_LEVELS);

    Buddy_item* item0 =BuddyAllocator_createItem(b_alloc, 0); 

}


    DATA_MAX BuddyAllocator_calcSize(DATA_MAX num_levels){
    DATA_MAX num_items=(1<<(num_levels+1));
    DATA_MAX list_alloc_size = (sizeof(Buddy_item)+sizeof(DATA_MAX))*num_items;
    return list_alloc_size;
    
}

Buddy_item* BuddyAllocator_createItem(BuddyAllocator* b_alloc, DATA_MAX idx){
    
    if(DEBUG){
        FILE* f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f, "Creating item...\n");
        fprintf(f, "Pool allocator location: %p\n", b_alloc->p_alloc); 
        fprintf(f, "Bucket size: %d\t", b_alloc->p_alloc->bucket_size); 
        fprintf(f, "Buffer addr: %p\t", b_alloc->p_alloc->buffer); 
        fprintf(f, "First idx: %d\n", b_alloc->p_alloc->first_idx); 
        fprintf(f, "First item free list: %p\n", b_alloc->p_alloc->free_list); 
        fprintf(f, "Item Size: %d\t", b_alloc->p_alloc->item_size);
        fprintf(f, "Buffer size: %d\t", b_alloc->p_alloc->size); 
        fprintf(f, "Buffer size max: %d\n", b_alloc->p_alloc->size_max);  
        fclose(f);
    }
    Buddy_item* item = PoolAllocator_getBlock(b_alloc->p_alloc);
    BitMap_setBit(b_alloc->tree->BitMap, idx, ALLOCATED);
    return item;
}

void BuddyAllocator_destroyItem(BuddyAllocator* b_alloc, Buddy_item* item){
    PoolAllocatorResult res = PoolAllocator_releaseBlock(b_alloc->p_alloc, item);
    assert(res==Success);
    //BitMap_setBit(b_alloc->tree->BitMap, item->idx, FREE);
}

Buddy_item* BuddyAllocator_getBuddy(BuddyAllocator* b_alloc, DATA_MAX level){
    if(DEBUG){
        FILE* f = fopen("OUT/Logs/log.txt", "a");
        fprintf(stdout, "Requested Buddy at level %d\n", level);
        fclose(f);   
    }
    if(level<0) return 0;
    assert(level<=b_alloc->num_levels);
    //Checks if there are buddies on the level, if not calls the same function on previous level
    if(!tree_buddiesOnLevel(b_alloc->tree, level)){
        Buddy_item* parent = BuddyAllocator_getBuddy(b_alloc, level-1);
        if(!parent) return NULL;
    }
    if(tree_buddiesOnLevel(b_alloc->tree, level)){
        printf("Buddies on level:\n");
        DATA_MAX idx = tree_first_free_node_level(b_alloc->tree, level);
        return (Buddy_item*)b_alloc->memory+(idx-(1<<tree_level(b_alloc->tree, idx)))*b_alloc->min_bucket_size;
    }
    assert(0);return(0);
}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, Buddy_item* item){
   
}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size){
    printf("Malloc\n");
    DATA_MAX mem_size = (1<<alloc->num_levels)*alloc->min_bucket_size;
    printf("%d\n", mem_size);
    DATA_MAX level = tree_level(alloc->tree, mem_size/(size+8));
    printf("alloc levels:%d, level: %d\n", alloc->num_levels, level);
    DATA_MAX idx = tree_first_free_node_level(alloc->tree, level);
    printf("%d\n", idx);
    if(level>alloc->num_levels) level = alloc->num_levels;
    if (DEBUG){
        FILE* f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f, "Requested Buddy of %d bytes, level %d\n",size, level);
        fclose(f);   
    }
    Buddy_item* buddy = BuddyAllocator_getBuddy(alloc, level);
    if(!buddy) return 0;
    Buddy_item** target = (Buddy_item**)(buddy->memory);
    *target = buddy;

    BitMap_setBit(alloc->tree->BitMap, idx, ALLOCATED);

    return buddy->memory+8;
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
        fprintf(f," |- memory available ....... [ %d ]\n",allocator_mem_size);
        fprintf(f," |- tree levels ............. [ %02d ]\n",num_levels);
        fprintf(f," |- tree nodes .............. [ %d ]\n",total_nodes);
        fprintf(f," |- leaf number ............. [ %d ]\n",leafs);
        fprintf(f," |- minimum-bs .............. [ %d ]\n",min_bucket_size);
        fprintf(f," |- total internal memory ... [ %d ]\n",internal_mem_required);
        fprintf(f," |- total user memory ....... [ %d ]\n",user_mem_required);
        fprintf(f," |- total required memory ... [ %d ]\n",total_mem_required);
        fprintf(f,"BuddyAllocator_initSingleBuffer|---------------------\n");
        if (total_mem_required > allocator_mem_size) {
            fprintf(f,"BuddyAllocator_initSingleBuffer|ERROR, invalid configuration\n");
            return;
        }
        fclose(f);
    }
    
    uint8_t *list_start = allocator_mem; 
    PoolAllocatorResult init_result = PoolAllocator_init(
        p_alloc, 
        sizeof(Buddy_item),
        total_nodes, 
        list_start, 
        internal_mem_required
    );
    if(DEBUG){
        FILE *f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f,"BuddyAllocator_initSingleBuffer|pool allocator result [ %s ]\n", PoolAllocator_strerror(init_result));
        fclose(f);
    }

    //ia we populate the fields of the buddy allocator
    b_alloc->num_levels = num_levels;
    b_alloc->min_bucket_size = min_bucket_size;
    b_alloc->memory = &allocator_mem[0]+internal_mem_required;
    b_alloc->buffer_size = allocator_mem_size;
    b_alloc->num_items = tree_nodes(num_levels);
    b_alloc->num_levels = num_levels;
    b_alloc->tree = tree;

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

    //Creates first item of the allocator
    Buddy_item *item = BuddyAllocator_createItem(b_alloc, 0);
}