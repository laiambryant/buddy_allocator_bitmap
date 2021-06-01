#include "../Bitmap.h"
#include "../Buddy.h"
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024*8  // 1 KByte Bitmap
#define DEBUG 1


int main(int argc, char const *argv[])
{
	uint8_t buffer[BUF_SIZE];
	BitMap *b = malloc(sizeof(BitMap));
	BitMap_init(b, BUF_SIZE, buffer);
	
	for(int i = 0; i<BUF_SIZE; i++){
		BitMap_setBit(b, i, ALLOCATED);
	}
	Bitmap_print(b,F_WRITE);

	for(int i = 0; i<BUF_SIZE; i++){
		BitMap_setBit(b, i, FREE);
	}
	Bitmap_print(b,F_CONCAT);

	free(b);
	return 0;
}
