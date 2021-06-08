#include <stdint.h>
#include <math.h>
#include <stdio.h>

#define DATA_MAX uint16_t

typedef struct Buddy_item{
    uint8_t *buffer;
    DATA_MAX idx;
}Buddy_item;

DATA_MAX get_idx(Buddy_item *bud);
DATA_MAX level(DATA_MAX idx);
DATA_MAX first_node_level(DATA_MAX idx);
DATA_MAX node_level_offset(DATA_MAX idx);
DATA_MAX buddy(DATA_MAX idx);
DATA_MAX parent(DATA_MAX idx);