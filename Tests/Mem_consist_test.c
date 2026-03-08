#include "../BuddyAllocator.h"
#include <assert.h>

#define LEVELS 9
#define BM_BUF_SIZE (1 << LEVELS)
#define BM_SIZE (BM_BUF_SIZE + sizeof(BitMap) + sizeof(BitMap_tree))
#define BALLOC_MEM_SIZE (1024 * 1024)
#define BALLOC_SIZE (BALLOC_MEM_SIZE + sizeof(BuddyAllocator) + BM_SIZE)
#define ALLOC_COUNT 32
#define ALLOC_SIZE 100

uint8_t BA_memory[BALLOC_SIZE];

int main(void) {
    memset(BA_memory, 0, BALLOC_SIZE);
    BuddyAllocator *b_alloc = BuddyAllocator_init(BA_memory, BALLOC_SIZE, LEVELS);
    DATA_MAX* ptrs[ALLOC_COUNT];
    for (int i = 0; i < ALLOC_COUNT; i++) {
        ptrs[i] = (DATA_MAX*)BuddyAllocator_malloc(b_alloc, ALLOC_SIZE);
        assert(ptrs[i] != NULL);
        *ptrs[i] = i;
    }
    int expected_sum = 0;
    for (int i = 0; i < ALLOC_COUNT; i++) {
        expected_sum += i;
    }
    int actual_sum = 0;
    for (int i = 0; i < ALLOC_COUNT; i++) {
        assert(*ptrs[i] == i);
        actual_sum += *ptrs[i];
    }
    assert(actual_sum == expected_sum);
    for (int i = 0; i < ALLOC_COUNT; i++) {
        BuddyAllocator_free(b_alloc, ptrs[i]);
    }
    return 0;
}
