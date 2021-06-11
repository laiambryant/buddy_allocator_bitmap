#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include "Bitmap.h"

#define DATA_MAX uint32_t

typedef struct Bitmap_tree{
    BitMap* BitMap;
    DATA_MAX levels;
}BitMap_tree;

typedef struct Buddy_item{
    uint8_t* memory;
    BitMap* BitMap;
    DATA_MAX idx;
    DATA_MAX parent_idx;
    DATA_MAX buddy_idx;
    DATA_MAX size;
    DATA_MAX level;
} Buddy_item;


DATA_MAX tree_get_idx(Buddy_item *bud);
DATA_MAX tree_level(DATA_MAX idx);
DATA_MAX tree_first_node_level(DATA_MAX idx);
DATA_MAX tree_node_level_offset(DATA_MAX idx);
DATA_MAX tree_getbuddy(DATA_MAX idx);
DATA_MAX tree_getparent(DATA_MAX idx);
//Checks if there are any set bits on the level of the tree
uint8_t tree_buddiesOnLevel(BitMap_tree *tree, DATA_MAX level);
void tree_print(BitMap_tree *tree, OUT_MODE out);