#include "BuddyAllocator.h"

#define MAX_LEVELS 16

//Buffer for bitmap
#define BM_BUF_SIZE 64*1024*8  // 64 KByte Bitmap
#define BM_SIZE 1*(BM_BUF_SIZE + sizeof(BitMap)) //Only 1 bitmap to save
uint8_t BM_memory[BM_SIZE];
uint8_t BM_buffer[BM_BUF_SIZE];

//Buffer for Buddy allocator
#define BALLOC_BUFF_SIZE 2*1024*1024*8 //2MB Memory Idxable
uint8_t BA_buffer[BALLOC_BUFF_SIZE];

int main(int argc, char const *argv[])
{
    PoolAllocator PAllocator;

	PoolAllocator_init(&PAllocator, sizeof(BitMap), 1, BM_memory, BM_SIZE);
	BitMap *b = (BitMap*) PoolAllocator_getBlock(&PAllocator);
	BitMap_init(b, BM_BUF_SIZE, BM_buffer);

    BuddyAllocator BAllocator;
    BuddyAllocator_init(b, &BAllocator, MAX_LEVELS, BA_buffer, BALLOC_BUFF_SIZE, BM_SIZE/(pow(2, MAX_LEVELS)));

    Bitmap_print(BAllocator.bitmap, 1);

    PoolAllocator_releaseBlock(&PAllocator, b);
    return 0;
}
