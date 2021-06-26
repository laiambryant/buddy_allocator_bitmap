#include "BuddyAllocator.h"
#define DEBUG 1

DATA_MAX BuddyAllocator_calcMinBucketSize(DATA_MAX buffer_size, DATA_MAX levels){
    return buffer_size/(pow(2, levels));
}

void BuddyAllocator_init(
    BitMap_tree* tree,
    BuddyAllocator* b_alloc,
    char* bm_buffer,
    char* memory,
    DATA_MAX buffer_size,
    DATA_MAX num_levels     ){
    
    
  // we need room also for level 0
  b_alloc->num_levels=num_levels;
  b_alloc->memory=memory;
  b_alloc->tree = tree;
  b_alloc->min_bucket_size=BuddyAllocator_calcMinBucketSize(buffer_size, num_levels);
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
  char *list_start=memory;
  PoolAllocatorResult init_result=PoolAllocator_init(&b_alloc->p_alloc,
						     sizeof(Buddy_item),
						     list_items,
						     list_start,
						     list_alloc_size);
  printf("%s\n",PoolAllocator_strerror(init_result));

   /*Buddy_item* item0 =BuddyAllocator_createItem(b_alloc, 0); 
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
*/
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
    if(level<0) return NULL;
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
    Buddy_item* parent = (Buddy_item*) alloc->memory[item->parent_idx];
    Buddy_item* buddy = (Buddy_item*) alloc->memory[item->buddy_idx];
    
    BitMap_setBit(alloc->tree->BitMap, item->idx, FREE);
    if(tree_getparent(item->idx)==item->idx)
        return;
    if(BitMap_bit(alloc->tree->BitMap, tree_getbuddy(item->idx)))
        return;

    BuddyAllocator_destroyItem(alloc, item);
    BuddyAllocator_destroyItem(alloc, buddy);
    BuddyAllocator_destroyItem(alloc, parent);

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

void BuddyAllocator_initSingleBuffer(){

}