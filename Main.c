#include "BuddyAllocator.h"
#include <string.h>

#define LEVELS 9
//Buffer for bitmap
#define BM_BUF_SIZE 1024*100 // 100 Kbit Bitmap
#define BM_SIZE 1*(BM_BUF_SIZE + sizeof(BitMap)) //Only 1 bitmap to save
uint8_t BM_buffer[BM_BUF_SIZE];

//Buffer for Buddy allocator
#define BALLOC_MEM_SIZE 1024*1024 //1Mbit Memory Idxable
uint8_t BA_memory[BALLOC_MEM_SIZE];

BuddyAllocator BAllocator;

int main(int argc, char const *argv[])
{
    //Initializes Pool allocator for bitmap and bitmap to store the tree
    PoolAllocator BM_PAllocator; 
    memset(BM_buffer, 0, BM_BUF_SIZE);
	PoolAllocatorResult res1 = PoolAllocator_init(
        &BM_PAllocator, 
        sizeof(BitMap), 
        1, 
        BM_buffer, 
        BM_SIZE); 
    assert(res1 == Success);
    BitMap *b = (BitMap*) PoolAllocator_getBlock(&BM_PAllocator);
	BitMap_init(b, BM_BUF_SIZE, BM_buffer);
    BitMap_tree tree = {b, LEVELS};

    memset(BA_memory, 0, BALLOC_MEM_SIZE);

    tree_print(&tree, F_CONCAT);

    printf("Min Bucket size: %d\t Num items: %d\n", BuddyAllocator_calcMinBucketSize(BALLOC_MEM_SIZE, LEVELS), (uint32_t)pow(2,LEVELS));

    BuddyAllocator_init(&tree, &BAllocator, BM_buffer,  BA_memory, BALLOC_MEM_SIZE, LEVELS);

    PoolAllocator_releaseBlock(&BM_PAllocator, b);
    return 0;
}
