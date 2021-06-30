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
    
   /* 
  // we need room also for level 0
  b_alloc->num_levels=num_levels;
  b_alloc->memory=memory;
  b_alloc->tree = tree;
  b_alloc->min_bucket_size; //TODOw
  b_alloc->buffer_size = buffer_size;
  b_alloc->num_items = 1<<(num_levels+1);
  assert (num_levels<MAX_LEVELS);
  // we need enough memory to handle internal structures
  assert (buffer_size>=BuddyAllocator_calcSize(num_levels));

  int list_items=1<<(num_levels+1); // maximum number of allocations, used to size the list
  int list_alloc_size=(sizeof(Buddy_item)+sizeof(int))*list_items;

    if(DEBUG){
        printf("Tree addr %p\n", b_alloc->tree);
        printf("Mem addr %p\n", b_alloc->memory);
        printf("Num levels %d\n", b_alloc->num_levels);
        printf("Min bucket size %d\n", b_alloc->min_bucket_size);
        printf("Buffer size %d\n", b_alloc->buffer_size);
        printf("Num items %d\n", b_alloc->num_items);
        printf("Managed memory %d bytes\n", (1<<num_levels)*b_alloc->min_bucket_size);
    }
  
    // the buffer for the list starts where the bitmap ends
    uint8_t *list_start= (uint8_t*)memory;
        PoolAllocatorResult init_result=PoolAllocator_init(b_alloc->p_alloc,
						     sizeof(Buddy_item),
						     list_items,
						     list_start,
						     list_alloc_size);
    printf("%s\n",PoolAllocator_strerror(init_result));

   Buddy_item* item0 =BuddyAllocator_createItem(b_alloc, 0); 
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
    */
}

Buddy_item* BuddyAllocator_createItem(BuddyAllocator* b_alloc, DATA_MAX idx){
    
    if(DEBUG){
        printf("Creating item...\n");
        printf("Pool allocator location: %p\n", b_alloc->p_alloc); 
        printf("Bucket size: %d\t", b_alloc->p_alloc->bucket_size); 
        printf("Buffer addr: %p\t", b_alloc->p_alloc->buffer); 
        printf("First idx: %d\n", b_alloc->p_alloc->first_idx); 
        printf("First item free list: %p\n", b_alloc->p_alloc->free_list); 
        printf("Item Size: %d\t", b_alloc->p_alloc->item_size);
        printf("Buffer size: %d\t", b_alloc->p_alloc->size); 
        printf("Buffer size max: %d\n", b_alloc->p_alloc->size_max);  
    }
    Buddy_item* item = PoolAllocator_getBlock(b_alloc->p_alloc);
    
    item->idx=idx;

    item->level = tree_level(idx);
    item->tree = b_alloc->tree;
    item->memory = b_alloc->memory + ((idx - (1 << tree_level(idx)))<< (b_alloc->num_levels - item->level)) *b_alloc->min_bucket_size;
    item->size =(1 << (b_alloc->num_levels - item->level)) * b_alloc->min_bucket_size;
    item->parent_idx = tree_getparent(idx);
    item->buddy_idx = 0;
    
    if(DEBUG){
        FILE* f = fopen("OUT/log.txt", "a");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(f,"Creating First item of Buddy allocator...\n");
        fprintf(f,"Item Metadata:\tBitmap Addr:\t%p,\t\tMemory start addr:\t%p\n", item->tree, item->memory);
        fprintf(f,"              \tItem idx:   \t%d,\t\tItem lvl:         \t%d\n", item->idx, item->level);
        fprintf(f,"              \tParent idx: \t%d,\t\tBuddy idx:        \t%d\n", item->parent_idx, item->buddy_idx);
        fprintf(f,"              \tItem size:  \t%d\n", item->size);
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fclose(f);
    }

    BitMap_setBit(b_alloc->tree->BitMap, idx, ALLOCATED);
    return item;
}

void BuddyAllocator_destroyItem(BuddyAllocator* b_alloc, Buddy_item* item){
    PoolAllocatorResult res = PoolAllocator_releaseBlock(b_alloc->p_alloc, item);
    assert(res==Success);
    BitMap_setBit(b_alloc->tree->BitMap, item->idx, FREE);
}

Buddy_item* BuddyAllocator_getBuddy(BuddyAllocator* b_alloc, DATA_MAX level){
    if(DEBUG){
        FILE* f = fopen("OUT/log.txt", "a");
        fprintf(stdout, "Requested Buddy at level %d\n", level);
        fclose(f);   
    }
    if(level<0) return 0;
    assert(level<=b_alloc->num_levels);
    //Checks if there are buddies on the level, if not calls the same function on previous level
    if(!tree_buddiesOnLevel(b_alloc->tree, level)){
        Buddy_item* parent = BuddyAllocator_getBuddy(b_alloc, level-1);
        if(!parent) return NULL;

        //If parent is already free
        //DATA_MAX left_idx = parent->idx<<1; DATA_MAX right_idx = parent->idx+1;

        //Buddy_item* left_ptr=BuddyAllocator_createItem(b_alloc, left_idx);
        //Buddy_item* right_ptr=BuddyAllocator_createItem(b_alloc, right_idx);
        //printf("left ptr: %p\n");
        //printf("right ptr: %p\n");
    }
    if(tree_buddiesOnLevel(b_alloc->tree, level)){
        printf("Buddies on level:\n");
        DATA_MAX idx = tree_first_free_node_level(b_alloc->tree, level);
        return (Buddy_item*)b_alloc->memory+(idx-(1<<tree_level(idx)))*b_alloc->min_bucket_size;
    }
    assert(0);return(0);
}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, Buddy_item* item){
    /*Buddy_item* parent = (Buddy_item*) alloc->memory[item->parent_idx];
    Buddy_item* buddy = (Buddy_item*) alloc->memory[item->buddy_idx];
    
    BitMap_setBit(alloc->tree->BitMap, item->idx, FREE);
    if(tree_getparent(item->idx)==item->idx)
        return;
    if(BitMap_bit(alloc->tree->BitMap, tree_getbuddy(item->idx)))
        return;

    BuddyAllocator_destroyItem(alloc, item);
    BuddyAllocator_destroyItem(alloc, buddy);
    BuddyAllocator_destroyItem(alloc, parent);
    */
}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size){
    
    DATA_MAX mem_size = (1<<alloc->num_levels)*alloc->min_bucket_size;
    DATA_MAX level = tree_level(mem_size/(size+8));
    if(level>alloc->num_levels) level = alloc->num_levels;
    if (DEBUG){
        FILE* f = fopen("OUT/log.txt", "a");
        fprintf(f, "Requested Buddy of %d bytes, level %d\n",size, level);
        fclose(f);   
    }

    Buddy_item* buddy = BuddyAllocator_getBuddy(alloc, level);

    if(!buddy) return 0;

    Buddy_item** target = (Buddy_item**)(buddy->memory);
    *target = buddy;
    return buddy->memory+8;
}

void BuddyAllocator_free(BuddyAllocator* alloc, void* mem){
    if(DEBUG){
        FILE* f = fopen("OUT/log.txt", "a");
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
        FILE* f = fopen("OUT/allocator_metadata.txt", "a");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
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
        FILE* f = fopen("OUT/allocator_metadata.txt", "w");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
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

void BuddyAllocator_initSingleBuffer(
        BuddyAllocator* alloc, 
        PoolAllocator* p_alloc,
        uint8_t* allocator_mem,
        BitMap_tree* tree,
        DATA_MAX allocator_mem_size,
        DATA_MAX num_levels){
    
    //Checks if allocator and memory buffer are correctly initialized and if memory buffer was
    assert(alloc);
    assert(allocator_mem_size!=0 && allocator_mem);

    alloc->p_alloc = p_alloc;

    const DATA_MAX total_nodes = tree_nodes(num_levels);
    const DATA_MAX internal_mem_required = (sizeof(Buddy_item)+sizeof(int))*total_nodes;
    const DATA_MAX user_mem = allocator_mem_size-internal_mem_required;
    const DATA_MAX min_bucket_size = (user_mem>>(num_levels));
    const DATA_MAX leaf_num = tree_leafs(num_levels);
    const DATA_MAX user_mem_required = min_bucket_size * leaf_num;
    const DATA_MAX total_mem_required = internal_mem_required + user_mem;

    if(DEBUG){
        fprintf(stdout, "\n----------------------------------------------------------------------------------------------\n");
        printf("BuddyAllocator_initSingleBuffer|current configuration\n");
        printf(" |- memory available ....... [ %ld ]\n",allocator_mem_size);
        printf(" |- tree levels ............. [ %02d ]\n",num_levels);
        printf(" |- tree nodes .............. [ %ld ]\n",total_nodes);
        printf(" |- leaf number ............. [ %ld ]\n",leaf_num);
        printf(" |- minimum-bs .............. [ %ld ]\n",min_bucket_size);
        printf(" |- total internal memory ... [ %ld ]\n",internal_mem_required);
        printf(" |- total user memory ....... [ %ld ]\n",user_mem_required);
        printf(" |- total required memory ... [ %ld ]\n",total_mem_required);
        printf("BuddyAllocator_initSingleBuffer|---------------------\n");
    }

    if(DEBUG){
        if (total_mem_required > allocator_mem_size) {
            printf("BuddyAllocator_initSingleBuffer|ERROR, invalid configuration\n");
            return;
        }
        printf("BuddyAllocator_initSingleBuffer|configuration valid, constructing allocator\n");
    }
    
    uint8_t *list_start = allocator_mem;
    PoolAllocatorResult init_result = PoolAllocator_init(
        p_alloc, 
        sizeof(Buddy_item),
        total_nodes, 
        list_start, 
        internal_mem_required
    );
    printf("BuddyAllocator_initSingleBuffer|pool allocator result [ %s ]\n",
        PoolAllocator_strerror(init_result));

    //ia we populate the fields of the buddy allocator
    alloc->num_levels = num_levels;
    alloc->min_bucket_size = min_bucket_size;
    alloc->memory = &allocator_mem[0] + internal_mem_required;
    alloc->buffer_size = allocator_mem_size;
    alloc->num_items = total_nodes;
    alloc->num_levels = num_levels;
    alloc->tree = tree;
    /*
    if (DEBUG){
        FILE *f = fopen("OUT/Mem.txt", "w");
        for (int i = 0; i<total_mem_required; i++){
            fprintf(f,"%x", alloc->memory[i]);
        }  
        fclose(f) ;
    }*/
    if (DEBUG){
        fprintf(stdout, "\n----------------------------------------------------------------------------------------------\n");
        printf("Initialized buffer...\n");
        printf("Item size: %d\n", alloc->p_alloc->item_size);
        printf("Items: %d\n", alloc->p_alloc->size);
        printf("Mem Block addr: %p\n", alloc->p_alloc->buffer);
        printf("Free list addr: %p\n", alloc->p_alloc->free_list);
        printf("Buffer size: %d\n", alloc->p_alloc->buffer_size);
        fprintf(stdout, "\n----------------------------------------------------------------------------------------------\n");
    }
    BuddyAllocator_createItem(alloc, 0);

}