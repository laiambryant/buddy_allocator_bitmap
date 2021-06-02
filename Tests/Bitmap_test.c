#include "../Bitmap.h"
#include "../pool_allocator.h"
#include <stdio.h>


#define BUF_SIZE 1024*8  // 1 KByte Bitmap
#define MEM_SIZE 1*(BUF_SIZE + sizeof(BitMap)) //Only 1 bitmap to save

char memory[MEM_SIZE];
uint8_t buffer[BUF_SIZE];

int main(int argc, char const *argv[]){

	PoolAllocator PAllocator;

	PoolAllocator_init(&PAllocator, sizeof(BitMap), 1, memory, MEM_SIZE);
	BitMap *b = (BitMap*) PoolAllocator_getBlock(&PAllocator);
	BitMap_init(b, BUF_SIZE, buffer);
	
	for(int i = 0; i<BUF_SIZE; i++){
		BitMap_setBit(b, i, ALLOCATED);
	}
	Bitmap_print(b,F_WRITE);


	for(int i = 0; i<BUF_SIZE; i++){
		BitMap_setBit(b, i, FREE);
	}
	Bitmap_print(b,F_CONCAT);

	PoolAllocator_releaseBlock(&PAllocator, b);
	return 0;
}
