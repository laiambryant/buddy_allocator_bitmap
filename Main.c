#include "BuddyAllocator.h"

#define LEVELS 9
#define BM_BUF_SIZE (1 << LEVELS)
#define BM_SIZE (BM_BUF_SIZE + sizeof(BitMap) + sizeof(BitMap_tree))
#define BALLOC_MEM_SIZE (1024 * 1024)
#define BALLOC_SIZE (BALLOC_MEM_SIZE + sizeof(BuddyAllocator) + BM_SIZE)
#define SMALL_ALLOC_COUNT 10

uint8_t BA_memory[BALLOC_SIZE];

int main(void) {
    memset(BA_memory, 0, BALLOC_SIZE);
    BuddyAllocator *b_alloc = BuddyAllocator_init(BA_memory, BALLOC_SIZE, LEVELS);
    int* large = (int*)BuddyAllocator_malloc(b_alloc, 30000);
    int* ptrs[SMALL_ALLOC_COUNT];
    for (int i = 0; i < SMALL_ALLOC_COUNT; i++) {
        ptrs[i] = (int*)BuddyAllocator_malloc(b_alloc, 100);
    }
    for (int i = 0; i < SMALL_ALLOC_COUNT; i++) {
        BuddyAllocator_free(b_alloc, ptrs[i]);
    }
    BuddyAllocator_free(b_alloc, large);
    int* large2 = (int*)BuddyAllocator_malloc(b_alloc, 30000);
    BuddyAllocator_free(b_alloc, large2);
    return 0;
}
