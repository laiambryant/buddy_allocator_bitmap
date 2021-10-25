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


//Given an index returns the level of that index in the tree
DATA_MAX tree_level(BitMap_tree* tree, DATA_MAX idx);
//Given an index returns first index of the level of that index
DATA_MAX tree_first_node_level(BitMap_tree* tree,DATA_MAX idx);
//Given a bitmap and a level returns index of first free node of that level, -1 if no free nodes
DATA_MAX tree_first_free_node_level(BitMap_tree* tree,DATA_MAX level);
//Given an index calculates the offset of the index in the node
DATA_MAX tree_node_level_offset(BitMap_tree* tree,DATA_MAX idx);
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
//Initializes bitmap tree buffer 
BitMap_tree* BitMap_tree_init(uint8_t* buffer, DATA_MAX buffer_size, DATA_MAX levels);
//checks if there are any free buddies on the level
DATA_MAX tree_free_buddies_on_level(BitMap_tree* tree, DATA_MAX level);
//Gets a free idx on the level
DATA_MAX tree_balloc_getIdx(BitMap_tree *tree, DATA_MAX level);
/*Sets parents, if status is FREE checks if buddy of idx is Free. If buddy is free goes up one level and frees parent idx.
If status is ALLOCATED, sets all parents*/
void tree_setParents(BitMap_tree* tree, DATA_MAX idx, DATA_MAX level, Status status);
//Calls internal method tree_setChildren_internal and sets all children.
void tree_setChildren(BitMap_tree* tree, DATA_MAX idx, Status status);
//Internal recursive func 
void tree_setChildren_internal(BitMap_tree* tree, DATA_MAX l_child, DATA_MAX r_child, Status status);
//Wrapper function for bitmap_setBit
void tree_setBit(BitMap_tree *tree, DATA_MAX bit_num, Status status);
//Wrapper function for bitmap_getBit
DATA_MAX tree_getBit(BitMap_tree *tree, DATA_MAX bit_num);