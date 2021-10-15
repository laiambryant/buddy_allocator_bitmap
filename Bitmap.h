#pragma once
#include "Bitmap.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#define DATA_MAX int
#define DEBUG 1

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
    uint8_t *Buf; 
    uint8_t *end_Buf;
    DATA_MAX buffer_size;
    DATA_MAX num_bits;
    DATA_MAX allocated_bits;
} BitMap;

// returns the number of bytes to store bits booleans
DATA_MAX BitMap_getBytes(DATA_MAX bits);

// initializes a bitmap on an external array
BitMap* BitMap_init(uint8_t* buffer, DATA_MAX buf_size);

// sets a the bit bit_num in the bitmap
// status= 0 or 1
DATA_MAX BitMap_setBit(BitMap *bit_map,  DATA_MAX bit_num, Status status);

// inspects the status of the bit bit_num
uint8_t BitMap_getBit( BitMap *bit_map,  DATA_MAX bit_num);

//Prints Bitmap
void Bitmap_print(BitMap *bit_map, OUT_MODE out_mode);
