#pragma once
#include "Bitmap.h"
#include "pool_allocator.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#define DATA_MAX int
#define DEBUG 1
/*
    Bitmap will work as following. Each time the buddy allocator creates the nodes of the tree the size of the bitmap will double.
    Implementation will have max 16 bit data types (can be changed changing the DATA_MAX const defined in preproccessing instruction)
    because it will be part of another project you can find here: https://github.com/laiambryant/NES_EMU
    If the block is already allocated the corresponding bit in the bitmap will be set to 1.
    EX. 
        If I have 2MB memory, and need to allocate <256Kb memory will be divided in 2 three times.
        2MB/2 1MB/2 512KB/2 256KB -> bitmap will be modified as follows:
        0     00    0000    00000000
    To understand @what level of the tree you are you find the exponent of //2^level=node_idx => floor(log_2(node_idx)) = level
    If i want to find parent node i must divide index of the child by 2.

*/

//Enum for bit status to avoid values != [0,1]
typedef enum Status{
    ALLOCATED = 0x01,
    FREE = 0x00
}Status;

typedef enum OUT_MODE{
    STDOUT = 0,  //stdout
    F_WRITE = 1, //fprintf w
    F_CONCAT = 2 //fprintf a
} OUT_MODE;

typedef struct BitMap{
    uint8_t *Buf; //Bitmap will be stored here
    uint8_t *end_Buf;
    DATA_MAX buffer_size;
    DATA_MAX num_bits;
    DATA_MAX allocated_bits;
} BitMap;

// returns the number of bytes to store bits booleans
DATA_MAX BitMap_getBytes(DATA_MAX bits);

// initializes a bitmap on an external array
BitMap* BitMap_init(PoolAllocator* p_alloc, DATA_MAX buf_size, uint8_t *buffer);

// sets a the bit bit_num in the bitmap
// status= 0 or 1
DATA_MAX BitMap_setBit(BitMap *bit_map,  DATA_MAX bit_num, Status status);

// inspects the status of the bit bit_num
uint8_t BitMap_bit( BitMap *bit_map,  DATA_MAX bit_num);

void Bitmap_print(BitMap *bit_map, OUT_MODE out_mode);

void BitMap_reset(BitMap* b);