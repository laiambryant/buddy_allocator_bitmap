#include "BuddyAllocator.h"
#define DEBUG 1

BuddyAllocator* BuddyAllocator_init(
    uint8_t* ba_buffer,
    DATA_MAX buffer_size,
    DATA_MAX num_levels){

    BuddyAllocator* b_alloc = (BuddyAllocator*)ba_buffer;
    uint8_t* tree_buff = ((uint8_t*)b_alloc) + sizeof(BuddyAllocator);
    DATA_MAX tree_mem_size =sizeof(BitMap)+sizeof(BitMap_tree)+(int)pow(2,num_levels);
    BitMap_tree* tree = BitMap_tree_init(tree_buff, tree_mem_size ,num_levels);

    const DATA_MAX balloc_mem_size = buffer_size-(sizeof(BuddyAllocator)+tree_mem_size);


    b_alloc->num_levels = num_levels;
    b_alloc->num_items = (int)pow(2,num_levels);
    b_alloc->memory = tree_buff+tree_mem_size;
    b_alloc->tree = tree;
    b_alloc->min_bucket_size = (balloc_mem_size>>(num_levels-1));;
    b_alloc->buffer_size = balloc_mem_size;
    b_alloc->leaf_num = b_alloc->buffer_size/b_alloc->min_bucket_size;
    b_alloc->user_mem=b_alloc->min_bucket_size*(b_alloc->leaf_num);
    
    assert (num_levels<MAX_LEVELS);

    if(DEBUG)   BuddyAllocator_printMetadata(b_alloc, F_WRITE);
    /*printf("BA_struct size:%d, Addr: %p\n",sizeof(BuddyAllocator),b_alloc);
    printf("Tree_struct size:%d, Addr: %p\n", sizeof(BitMap_tree), tree);
    printf("BM_struct size:%d, Addr: %p\n", sizeof(BitMap), tree->BitMap);
    printf("BM size:%d, Addr: %p\n", (int)pow(2,num_levels), tree->BitMap->Buf);
    printf("Mem size:%d, Addr: %p\n", b_alloc->user_mem, b_alloc->memory);*/
    
    return b_alloc;

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

    DATA_MAX offset = ((uint8_t*)item-sizeof(DATA_MAX))-alloc->memory;
    DATA_MAX max_offset = alloc->user_mem;
    //Error Handling
    if (offset>max_offset)
        return BA_OutOfRange;

    //Fetches idx pointer from the address (subtracting the size of the the datatype chosen)
    DATA_MAX* idx_ptr = (DATA_MAX*)(item-sizeof(DATA_MAX));
    DATA_MAX idx = *idx_ptr; 

    if(DEBUG){
        FILE* f = fopen("OUT/Logs/log.txt", "a");
        fprintf(f, "Freeing memory: @%p\t", idx_ptr);
        fprintf(f,"Item idx: [%d], num_items[%d], offset[%d]\n",  idx, alloc->num_items, offset);
        fclose(f);   
    }


    if (offset%alloc->min_bucket_size)
        return BA_UnalignedFree;
    if(tree_getBit(alloc->tree, idx)==FREE) 
        return BA_DoubleFree;

    tree_setBit(alloc->tree, idx, FREE);
    //Parents will be freed only if buddy of the node is also free 
    tree_setParents(alloc->tree, idx, tree_level(alloc->tree, idx), FREE);
    tree_setChildren(alloc->tree, idx, FREE);
    if(DEBUG) 
        tree_print(alloc->tree, F_CONCAT);

    return BA_Success;    
}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size){
    
    //Leave space (size of pointer) to write addr of target mem
    DATA_MAX level = tree_level(alloc->tree, alloc->user_mem/(size+sizeof(DATA_MAX)));
    if(level>alloc->num_levels) level = alloc->num_levels;
    
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

    //Checks if pointer is not null and calls release buddy
    if(mem!=NULL){
        BuddyAllocatorResult res = BuddyAllocator_releaseBuddy(alloc, mem);
        if(DEBUG){
            FILE* f = fopen("OUT/Logs/log.txt", "a");
            fprintf(f, "Result: %s\n", BuddyAllocator_strerror(res));
            fclose(f);
        }
    }else{
        printf("[FREE][Error, NULL pointer]: exiting...\n");
        assert(0);
    }

}

void BuddyAllocator_printMetadata(BuddyAllocator* b_alloc, OUT_MODE out){
    if(out==STDOUT){
        fprintf(stdout, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(stdout,"\tAlloc Address: %p\n", b_alloc);
        fprintf(stdout,"\tTree Address: %p\n", b_alloc->tree);
        fprintf(stdout ,"\tBitmap Address: %p\n", b_alloc->tree->BitMap);
        fprintf(stdout ,"\tBuddy Allocator levels: %d\n", b_alloc->num_levels);
        fprintf(stdout ,"\tBuddy Allocator leafs: %d\n", b_alloc->leaf_num);
        fprintf(stdout ,"\tBuddy Allocator min bucket size: %d\n", b_alloc->min_bucket_size);
        fprintf(stdout ,"\tMem alloc size: %d\n", b_alloc->buffer_size);
        fprintf(stdout ,"\tUser mem: %d\n", b_alloc->user_mem);
        fprintf(stdout ,"\tMem address start: %p\n", b_alloc->memory);
        fprintf(stdout ,"\tMem address end: %p\n", (b_alloc->memory)+b_alloc->buffer_size);
        fprintf(stdout, "\n----------------------------------------------------------------------------------------------\n");
    }
    if(out==F_CONCAT){
        FILE* f = fopen("OUT/Logs/allocator_metadata.txt", "a");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(f,"\tAlloc Address: %p\n", b_alloc);
        fprintf(f,"\tTree Address: %p\n", b_alloc->tree);
        fprintf(f,"\tBitmap Address: %p\n", b_alloc->tree->BitMap);
        fprintf(f,"\tBuddy Allocator levels: %d\n", b_alloc->num_levels);
        fprintf(f,"\tBuddy Allocator leafs: %d\n", b_alloc->leaf_num);
        fprintf(f,"\tBuddy Allocator min bucket size: %d\n", b_alloc->min_bucket_size);
        fprintf(f,"\tMem alloc size: %d\n", b_alloc->buffer_size);
        fprintf(f,"\tUser mem: %d\n", b_alloc->user_mem);
        fprintf(f,"\tMem address start: %p\n", b_alloc->memory);
        fprintf(f,"\tMem address end: %p\n", (b_alloc->memory)+b_alloc->buffer_size);
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fclose(f);
    }
    if(out==F_WRITE){
        FILE* f = fopen("OUT/Logs/allocator_metadata.txt", "w");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(f,"\tAlloc Address: %p\n", b_alloc);
        fprintf(f,"\tTree Address: %p\n", b_alloc->tree);
        fprintf(f,"\tBitmap Address: %p\n", b_alloc->tree->BitMap);
        fprintf(f,"\tBuddy Allocator levels: %d\n", b_alloc->num_levels);
        fprintf(f,"\tBuddy Allocator leafs: %d\n", b_alloc->leaf_num);
        fprintf(f,"\tBuddy Allocator min bucket size: %d\n", b_alloc->min_bucket_size);
        fprintf(f,"\tMem alloc size: %d\n", b_alloc->buffer_size);
        fprintf(f,"\tUser mem: %d\n", b_alloc->user_mem);
        fprintf(f,"\tMem address start: %p\n", b_alloc->memory);
        fprintf(f,"\tMem address end: %p\n", (b_alloc->memory)+b_alloc->buffer_size);
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fclose(f);
    }
}


const char* BuddyAllocator_strerror(BuddyAllocatorResult result) {
  return BuddyAllocator_strerrors[result];
}

DATA_MAX BuddyAllocator_calcSize(DATA_MAX num_levels){
    DATA_MAX num_items=(1<<(num_levels+1))-1;
    DATA_MAX list_alloc_size = num_items;
    return list_alloc_size;
}
