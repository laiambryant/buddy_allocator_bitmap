#include "Bitmap.h"

BitMap* BitMap_init(uint8_t *buffer, DATA_MAX buf_size) {
    assert(buffer != NULL);
    BitMap* bit_map = (BitMap*) buffer;
    uint8_t* bm_mem_start = ((uint8_t*)buffer) + sizeof(BitMap);
    bit_map->Buf = bm_mem_start;
    bit_map->num_bits = buf_size;
    bit_map->buffer_size = BitMap_getBytes(bit_map->num_bits);
    bit_map->end_Buf = buffer + bit_map->buffer_size;
    for (DATA_MAX i = 0; i < buf_size; i++) {
        BitMap_setBit(bit_map, i, FREE);
    }
    return bit_map;
}

DATA_MAX BitMap_setBit(BitMap *bit_map, DATA_MAX bit_num, Status status) {
    if (bit_num < 0) return 0;
    DATA_MAX page = bit_num >> 3;
    assert(page < bit_map->buffer_size);
    DATA_MAX offset = bit_num % 8;
    if (status == ALLOCATED)
        bit_map->Buf[page] |= (1U << offset);
    if (status == FREE)
        bit_map->Buf[page] &= ~(1U << offset);
    return 1;
}

uint8_t BitMap_getBit(BitMap *bit_map, DATA_MAX bit_num) {
    DATA_MAX page = bit_num >> 3;
    assert(page < bit_map->buffer_size);
    DATA_MAX offset = bit_num % 8;
    return (bit_map->Buf[page] & (1U << offset)) != 0;
}

void Bitmap_print(BitMap *bit_map, FILE *out) {
    if (bit_map->Buf == NULL) return;
    fprintf(out, "\n----------------------------------------------------------------------------------------------\n");
    fprintf(out, "Bitmap Metadata:\n");
    fprintf(out, "%d bits\t%d bytes\n", bit_map->num_bits, bit_map->buffer_size);
    fprintf(out, "%p start\t%p end\n", (void*)bit_map->Buf, (void*)bit_map->end_Buf);
    fprintf(out, "Bitmap STATUS:\n");
    for (int i = 0; i < bit_map->num_bits; i++) {
        fprintf(out, "%x", BitMap_getBit(bit_map, i));
    }
    fprintf(out, "\n----------------------------------------------------------------------------------------------\n");
}

DATA_MAX BitMap_getBytes(DATA_MAX bits) {
    return ((bits >> 3) + ((bits % 8) != 0));
}
