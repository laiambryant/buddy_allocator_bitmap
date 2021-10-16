#include "../BuddyAllocator.h"
#include <string.h>

#define LEVELS 10
//Buffer for bitmap
#define BM_BUF_SIZE 1024// 512 bit Bitmap
#define BM_SIZE BM_BUF_SIZE + sizeof(BitMap) + sizeof(BitMap_tree)//Only 1 bitmap to save
uint8_t BM_buffer[BM_SIZE];

//Buffer for Buddy allocator
#define BALLOC_MEM_SIZE 2*1024*1024 //1Mbit Memory Idxable
#define BALLOC_SIZE BALLOC_MEM_SIZE+sizeof(BuddyAllocator)+BM_SIZE
uint8_t BA_memory[BALLOC_SIZE];

int main(int argc, char const *argv[]){
    BitMap_tree *tree = BitMap_tree_init(BM_buffer, BM_SIZE, LEVELS);
    printf("Addr: %p\n", tree);
    tree_print(tree, F_WRITE);

    BuddyAllocator *b_alloc = BuddyAllocator_init(
        BA_memory, 
        BALLOC_SIZE,
        LEVELS
    );

    printf("Addr: %p\n", b_alloc);
    
    

    return 0;
}
