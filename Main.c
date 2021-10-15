#include "BuddyAllocator.h"
#include <string.h>

#define LEVELS 9
#define BM_BUF_SIZE 512 //pow(2, LEVELS)
#define BM_SIZE BM_BUF_SIZE + sizeof(BitMap) + sizeof(BitMap_tree)//Only 1 bitmap to save

//Buffer for Buddy allocator
#define BALLOC_MEM_SIZE 1024*1024 //1Mbit Memory Idxable
#define BALLOC_SIZE BALLOC_MEM_SIZE+sizeof(BuddyAllocator)+BM_SIZE
uint8_t BA_memory[BALLOC_SIZE];

int main(int argc, char const *argv[])
{
    //Sets all bits of bitmap and buddy allocator memory to 0
    memset(BA_memory, 0, BALLOC_MEM_SIZE);

    //Initializes bitmap
    BuddyAllocator *b_alloc = BuddyAllocator_init(
        BA_memory, BALLOC_SIZE, LEVELS
    );
        
    int* var_ptr = (int*) BuddyAllocator_malloc(b_alloc,30000);
    
    for (int i = 0; i<100; i++) {
        int* mem = (int*)BuddyAllocator_malloc(b_alloc,100);
    }   

    int* var_ptr1 = (int*) BuddyAllocator_malloc(b_alloc,30000);

    BuddyAllocator_free(b_alloc, var_ptr);

    int* var_ptr2 = (int*) BuddyAllocator_malloc(b_alloc,30000);


    return 0;
}
