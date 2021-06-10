#include "../Bitmap.h"
#include "../Bitmap_tree.h"
#include "../pool_allocator.h"
#include <stdio.h>


#define BUF_SIZE 1024*8  // 1 KByte Bitmap
#define MEM_SIZE (BUF_SIZE + sizeof(BitMap)) //Only 1 bitmap to save

uint8_t buffer[MEM_SIZE];
uint8_t memory[BUF_SIZE];

int main(int argc, char const *argv[]){

	PoolAllocator PAllocator;

	PoolAllocator_init(&PAllocator, sizeof(BitMap), 1, buffer, MEM_SIZE);
	BitMap *b = (BitMap*) PoolAllocator_getBlock(&PAllocator);
	BitMap_init(b, BUF_SIZE, memory);

	for(uint32_t i = 0; i<BUF_SIZE; i++){
		BitMap_setBit(b, i, ALLOCATED);
	}
	Bitmap_print(b,F_WRITE);

	for(uint32_t j = 0; j<BUF_SIZE; j++){
		BitMap_setBit(b, j, FREE);
	}
	Bitmap_print(b,F_CONCAT);

	BitMap_setBit(b, 1, ALLOCATED);
	Bitmap_print(b,F_CONCAT);

	PoolAllocator_releaseBlock(&PAllocator, b);

	return 0;
}
