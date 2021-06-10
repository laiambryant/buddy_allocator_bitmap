#include "Bitmap_tree.h"

DATA_MAX tree_get_idx(Buddy_item *bud){
    if(bud){
        return bud->idx;
    }
    else return -1;
}

DATA_MAX tree_level(DATA_MAX idx){
    return (DATA_MAX) floor(log2(idx)); //2^level=node_idx => floor(log_2(node_idx)) = level
}
DATA_MAX tree_first_node_level(DATA_MAX idx){
    return 0x0001<<tree_level(idx);
}
DATA_MAX tree_node_level_offset(DATA_MAX idx){
    return idx-tree_first_node_level(tree_level(idx));
}
DATA_MAX tree_getbuddy(DATA_MAX idx){
    return 
        (idx & 0x0001)?(idx-1):(idx);
}
DATA_MAX tree_getparent(DATA_MAX idx){
    return (uint16_t)idx/2;
}

void tree_print(BitMap_tree *tree){
    for (int i = 0; i < tree->levels; i++){
		for (int j = 0; j < pow(2,i);j++){
		    printf("%d",BitMap_bit(tree->BitMap,pow(2,i)+j));
	    }
		printf("\n");
	}
}