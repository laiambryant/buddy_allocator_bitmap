#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include "Bitmap.h"

#define DATA_MAX int

typedef struct Bitmap_tree{
    BitMap* BitMap;
    DATA_MAX levels;
}BitMap_tree;

typedef struct Buddy_item{
    char* memory;
    BitMap_tree* tree;
    DATA_MAX idx;
    DATA_MAX parent_idx;
    DATA_MAX buddy_idx;
    DATA_MAX size;
    DATA_MAX level;
}Buddy_item;



//Given an item returns the index for that item or returns -1 if bud is not initialized
DATA_MAX tree_get_idx(Buddy_item *bud);
//Given an index returns the level of that index in the tree
DATA_MAX tree_level(DATA_MAX idx);
//Given an index returns first index of the level of that index
DATA_MAX tree_first_node_level(DATA_MAX idx);
//Given a bitmap and a level returns 
DATA_MAX tree_first_free_node_level(BitMap_tree* tree,DATA_MAX level);
DATA_MAX tree_node_level_offset(DATA_MAX idx);
DATA_MAX tree_getbuddy(DATA_MAX idx);
DATA_MAX tree_getparent(DATA_MAX idx);
//Checks if there are any set bits on the level of the tree
DATA_MAX tree_buddiesOnLevel(BitMap_tree *tree, DATA_MAX level);
void tree_print(BitMap_tree *tree, OUT_MODE out);
DATA_MAX tree_nodes(DATA_MAX levels);
DATA_MAX tree_leafs(DATA_MAX levels);