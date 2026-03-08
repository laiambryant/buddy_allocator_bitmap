#include "../Bitmap.h"
#include <assert.h>

#define BUF_SIZE 256

uint8_t buffer[BUF_SIZE + sizeof(BitMap)];

int main(void) {
    BitMap *b = BitMap_init(buffer, BUF_SIZE);
    for (DATA_MAX i = 0; i < BUF_SIZE; i++) {
        BitMap_setBit(b, i, ALLOCATED);
    }
    for (DATA_MAX i = 0; i < BUF_SIZE; i++) {
        assert(BitMap_getBit(b, i) == ALLOCATED);
    }
    for (DATA_MAX i = 0; i < BUF_SIZE; i++) {
        BitMap_setBit(b, i, FREE);
    }
    for (DATA_MAX i = 0; i < BUF_SIZE; i++) {
        assert(BitMap_getBit(b, i) == FREE);
    }
    BitMap_setBit(b, 7, ALLOCATED);
    assert(BitMap_getBit(b, 7) == ALLOCATED);
    assert(BitMap_getBit(b, 6) == FREE);
    assert(BitMap_getBit(b, 8) == FREE);
    BitMap_setBit(b, 7, FREE);
    assert(BitMap_getBit(b, 7) == FREE);
    return 0;
}
