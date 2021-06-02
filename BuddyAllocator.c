#include "BuddyAllocator.h"

DATA_MAX BuddyAllocator_calcSize(DATA_MAX num_levels){

}

void BuddyAllocator_init(BuddyAllocator* alloc,
                         DATA_MAX num_levels,
                         uint8_t* buffer,
                         DATA_MAX buffer_size,
                         uint8_t* memory,
                         DATA_MAX min_bucket_size){

}

BuddyListItem* BuddyAllocator_getBuddy(BuddyAllocator* alloc, DATA_MAX level){

}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, BuddyListItem* item){

}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size){

}

void BuddyAllocator_free(BuddyAllocator* alloc, void* mem){

}

DATA_MAX level(DATA_MAX tree_node){
    assert(tree_node>0);
    return (DATA_MAX) floor(log2(tree_node)); //2^level=node_idx => floor(log_2(node_idx)) = level
}


DATA_MAX first_node_level(DATA_MAX tree_node){
    assert(level>0);
    return 0x0001<<level(tree_node);
}

DATA_MAX node_level_offset(DATA_MAX tree_node){
    assert(level>0);
    return tree_node-first_node_level(level(tree_node));
}

DATA_MAX buddy(DATA_MAX tree_node){
    assert(tree_node>0);
    return 
        (tree_node & 0x0001)?(tree_node-1):(tree_node);
}
DATA_MAX parent(DATA_MAX tree_node){
    assert(tree_node>0);
    return (uint16_t)tree_node/2;
}