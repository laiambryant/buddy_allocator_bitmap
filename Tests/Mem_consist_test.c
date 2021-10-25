#include "../BuddyAllocator.h"
#include <string.h>

#define LEVELS 9
//Buffer for bitmap
#define BM_BUF_SIZE 512// 512 bit Bitmap
#define BM_SIZE BM_BUF_SIZE + sizeof(BitMap) + sizeof(BitMap_tree)//Only 1 bitmap to save
uint8_t BM_buffer[BM_SIZE];

//Buffer for Buddy allocator
#define BALLOC_MEM_SIZE 1024*1024 //1Mbit Memory Idxable
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

    DATA_MAX* ptrs[256];

    for (int i=0; i<256; i++){
        ptrs[i] = (DATA_MAX*)BuddyAllocator_malloc(b_alloc, 100);
        *ptrs[i] = i; 
    }

    for(int j=0; j<256; j++){
        printf("%d\t", *ptrs[j]);
    }
    printf("\n");
  
    return 0;
}
