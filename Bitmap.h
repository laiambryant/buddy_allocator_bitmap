#pragma once
#include "common.h"

typedef struct BitMap {
    uint8_t *Buf;
    uint8_t *end_Buf;
    DATA_MAX buffer_size;
    DATA_MAX num_bits;
    DATA_MAX allocated_bits;
} BitMap;

DATA_MAX BitMap_getBytes(DATA_MAX bits);
BitMap* BitMap_init(uint8_t* buffer, DATA_MAX buf_size);
DATA_MAX BitMap_setBit(BitMap *bit_map, DATA_MAX bit_num, Status status);
uint8_t BitMap_getBit(BitMap *bit_map, DATA_MAX bit_num);
void Bitmap_print(BitMap *bit_map, FILE *out);
