#include "../BuddyAllocator.h"
#include <assert.h>

#define LEVELS 9
#define BM_BUF_SIZE (1 << LEVELS)
#define BM_SIZE (BM_BUF_SIZE + sizeof(BitMap) + sizeof(BitMap_tree))
#define BALLOC_MEM_SIZE (1024 * 1024)
#define BALLOC_SIZE (BALLOC_MEM_SIZE + sizeof(BuddyAllocator) + BM_SIZE)

uint8_t BA_memory[BALLOC_SIZE];

int main(void) {
    memset(BA_memory, 0, BALLOC_SIZE);
    BuddyAllocator *b_alloc = BuddyAllocator_init(BA_memory, BALLOC_SIZE, LEVELS);
    assert(b_alloc != NULL);
    assert(b_alloc->num_levels == LEVELS);
    void *ptr = BuddyAllocator_malloc(b_alloc, b_alloc->user_mem / 2);
    assert(ptr != NULL);
    BuddyAllocator_free(b_alloc, ptr);
    return 0;
}
