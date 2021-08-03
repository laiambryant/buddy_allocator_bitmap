#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include "Bitmap.h"

#define DATA_MAX int

//A struct that holds a bitmap. 
typedef struct Bitmap_tree{
    BitMap* BitMap;
    DATA_MAX levels;
    DATA_MAX total_nodes;
    DATA_MAX leaf_num;
}BitMap_tree;

typedef struct Buddy_item{
    uint8_t* memory;
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
//Given an index calculates the offset of the index in the node
DATA_MAX tree_node_level_offset(DATA_MAX idx);
//Given an index calculates the the index of the buddy
DATA_MAX tree_getbuddy(DATA_MAX idx);
//Given an index calculates the index of the parent
DATA_MAX tree_getparent(DATA_MAX idx);
//Checks if there are any set bits on the level of the tree
DATA_MAX tree_buddiesOnLevel(BitMap_tree *tree, DATA_MAX level);
//Prints tree, out can be F_WRITE, F_CONCAT (that respectively call fprintf with "w" and "a" flags), STDOUT calls fprintf on stdout
void tree_print(BitMap_tree *tree, OUT_MODE out);
//Calculates number of nodes given the number of levels
DATA_MAX tree_nodes(DATA_MAX levels);
//Calculates number of leafs given the number of levels
DATA_MAX tree_leafs(DATA_MAX levels);
//Initializes bitmap tree
BitMap_tree* BitMap_tree_init(BitMap_tree *tree, BitMap* b, DATA_MAX levels);