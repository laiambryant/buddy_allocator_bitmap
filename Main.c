#include "BuddyAllocator.h"
#include <string.h>

#define LEVELS 9
//Buffer for bitmap
#define BM_BUF_SIZE 512// 512 bit Bitmap
#define BM_SIZE 1*(BM_BUF_SIZE + sizeof(BitMap)) //Only 1 bitmap to save
uint8_t BM_buffer[BM_BUF_SIZE];

//Buffer for Buddy allocator
#define BALLOC_MEM_SIZE 1024*1024 //1Mbit Memory Idxable
uint8_t BA_memory[BALLOC_MEM_SIZE];

BuddyAllocator BAllocator;

int main(int argc, char const *argv[])
{
    PoolAllocator BM_PAllocator, BA_Pallocator; 
    //Sets all bits of bitmap and buddy allocator memory to 0
    memset(BM_buffer, 0, BM_BUF_SIZE);
    memset(BA_memory, 0, BALLOC_MEM_SIZE);
    //Initializes bitmap
	BitMap *b = BitMap_init(&BM_PAllocator, BM_BUF_SIZE, BM_buffer);
    BitMap_tree tree = {
        b,
        LEVELS,
        tree_nodes(LEVELS),
        tree_leafs(LEVELS),
    };
    tree_print(&tree, F_WRITE);
    BuddyAllocator_initSingleBuffer(&BAllocator, &BA_Pallocator, BA_memory, &tree, BALLOC_MEM_SIZE, LEVELS);
    tree_print(&tree, F_CONCAT);
    BuddyAllocator_printMetadata(&BAllocator, F_WRITE);

    return 0;
}
