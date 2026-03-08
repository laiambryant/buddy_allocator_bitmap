#pragma once
#include "Bitmap.h"

typedef struct Bitmap_tree {
    BitMap* BitMap;
    DATA_MAX levels;
    DATA_MAX total_nodes;
    DATA_MAX leaf_num;
} BitMap_tree;

DATA_MAX tree_level(BitMap_tree* tree, DATA_MAX idx);
DATA_MAX tree_first_node_level(BitMap_tree* tree, DATA_MAX idx);
DATA_MAX tree_first_free_node_level(BitMap_tree* tree, DATA_MAX level);
DATA_MAX tree_node_level_offset(BitMap_tree* tree, DATA_MAX idx);
DATA_MAX tree_getbuddy(DATA_MAX idx);
DATA_MAX tree_getparent(DATA_MAX idx);
DATA_MAX tree_buddiesOnLevel(BitMap_tree *tree, DATA_MAX level);
void tree_print(BitMap_tree *tree, FILE *out);
DATA_MAX tree_nodes(DATA_MAX levels);
DATA_MAX tree_leafs(DATA_MAX levels);
BitMap_tree* BitMap_tree_init(uint8_t* buffer, DATA_MAX buffer_size, DATA_MAX levels);
DATA_MAX tree_free_buddies_on_level(BitMap_tree* tree, DATA_MAX level);
DATA_MAX tree_balloc_getIdx(BitMap_tree *tree, DATA_MAX level);
void tree_setParents(BitMap_tree* tree, DATA_MAX idx, DATA_MAX level, Status status);
void tree_setChildren(BitMap_tree* tree, DATA_MAX idx, Status status);
void tree_setChildren_internal(BitMap_tree* tree, DATA_MAX l_child, DATA_MAX r_child, Status status);
void tree_setBit(BitMap_tree *tree, DATA_MAX bit_num, Status status);
DATA_MAX tree_getBit(BitMap_tree *tree, DATA_MAX bit_num);
