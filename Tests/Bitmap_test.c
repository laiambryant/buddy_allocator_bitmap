#include "../Bitmap.h"
#include "../Bitmap_tree.h"
#include <stdio.h>


#define BUF_SIZE 1024*8  // 1 KByte Bitmap
#define MEM_SIZE (BUF_SIZE + sizeof(BitMap)) //Only 1 bitmap to save

#define LEVELS 13

uint8_t buffer[MEM_SIZE];
uint8_t memory[BUF_SIZE];

int main(int argc, char const *argv[]){


	BitMap *b = BitMap_init(memory, MEM_SIZE);

	for(DATA_MAX i = 0; i<BUF_SIZE; i++){
		BitMap_setBit(b, i, ALLOCATED);
	}
	Bitmap_print(b,F_WRITE);

	for(DATA_MAX j = 0; j<BUF_SIZE; j++){
		BitMap_setBit(b, j, FREE);
	}
	Bitmap_print(b,F_CONCAT);

	BitMap_setBit(b, 7, ALLOCATED);
	Bitmap_print(b,F_CONCAT);
	BitMap_setBit(b, 7, FREE);
	Bitmap_print(b, F_CONCAT);

	BitMap_tree tree = {
		b,
    	LEVELS,
		tree_nodes(LEVELS),
		tree_leafs(LEVELS)
	}; 

	tree_print(&tree, F_CONCAT);

	for(DATA_MAX j = 0; j<BUF_SIZE; j++){
		if((j%2))BitMap_setBit(b, j, FREE);
		else BitMap_setBit(b, j, ALLOCATED);
	}
	tree_print(&tree, F_CONCAT);

	return 0;
}
