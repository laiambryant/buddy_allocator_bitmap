#include "Buddy.h"

DATA_MAX get_idx(Buddy_item *bud){
    if(bud){
        return bud->idx;
    }
    else return -1;
}

DATA_MAX level(DATA_MAX idx){
    return (DATA_MAX) floor(log2(idx)); //2^level=node_idx => floor(log_2(node_idx)) = level
}

DATA_MAX first_node_level(DATA_MAX idx){
    return 0x0001<<level(idx);
}

DATA_MAX node_level_offset(DATA_MAX idx){
    return idx-first_node_level(level(idx));
}

DATA_MAX buddy(DATA_MAX idx){
    return 
        (idx & 0x0001)?(idx-1):(idx);
}
DATA_MAX parent(DATA_MAX idx){
    return (uint16_t)idx/2;
}
