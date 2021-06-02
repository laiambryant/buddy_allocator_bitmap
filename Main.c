#include "BuddyAllocator.h"

#define BUF_SIZE 1024*8  // 1 KByte Bitmap
#define BM_SIZE 1*(BUF_SIZE + sizeof(BitMap)) //Only 1 bitmap to save

char BM_memory[BM_SIZE];
uint8_t buffer[BUF_SIZE];

int main(int argc, char const *argv[])
{
    PoolAllocator PAllocator;

	PoolAllocator_init(&PAllocator, sizeof(BitMap), 1, BM_memory, BM_SIZE);
	BitMap *b = (BitMap*) PoolAllocator_getBlock(&PAllocator);
	BitMap_init(b, BUF_SIZE, buffer);

    PoolAllocator_releaseBlock(&PAllocator, b);
    return 0;
}
