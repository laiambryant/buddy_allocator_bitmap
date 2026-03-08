#include "../BuddyAllocator.h"

#define LEVELS 10
#define BM_BUF_SIZE (1 << LEVELS)
#define BM_SIZE (BM_BUF_SIZE + sizeof(BitMap) + sizeof(BitMap_tree))
#define BALLOC_MEM_SIZE (1024 * 1024)
#define BALLOC_SIZE (BALLOC_MEM_SIZE + sizeof(BuddyAllocator) + BM_SIZE)

uint8_t BA_memory[BALLOC_SIZE];

void createError_BA_NotEnoughMemory(BuddyAllocator *b_alloc);
void createError_BA_UnalignedFree(BuddyAllocator *b_alloc);
void createError_BA_OutOfRange(BuddyAllocator *b_alloc);
void createError_BA_DoubleFree(BuddyAllocator *b_alloc);

int main(int argc, char const *argv[]) {
    if (argc < 2) return 1;
    memset(BA_memory, 0, BALLOC_SIZE);
    BuddyAllocator *b_alloc = BuddyAllocator_init(BA_memory, BALLOC_SIZE, LEVELS);
    int test = *argv[1] - '0';
    switch (test) {
    case 1:
        printf("No Mem\n");
        createError_BA_NotEnoughMemory(b_alloc);
        break;
    case 2:
        printf("Out Of Range\n");
        createError_BA_OutOfRange(b_alloc);
        break;
    case 3:
        printf("Unaligned Free\n");
        createError_BA_UnalignedFree(b_alloc);
        break;
    case 4:
        printf("Double Free\n");
        createError_BA_DoubleFree(b_alloc);
        break;
    default:
        break;
    }
    return 0;
}

void createError_BA_NotEnoughMemory(BuddyAllocator *b_alloc) {
    for (int i = 0; i < BM_BUF_SIZE / 2; i++) {
        BuddyAllocator_malloc(b_alloc, 100);
    }
    printf("Mem is now completely allocated...\n");
    BuddyAllocator_malloc(b_alloc, 100);
}

void createError_BA_UnalignedFree(BuddyAllocator *b_alloc) {
    int* test = (int*)BuddyAllocator_malloc(b_alloc, 100);
    test = test + 1;
    BuddyAllocator_free(b_alloc, test);
}

void createError_BA_OutOfRange(BuddyAllocator *b_alloc) {
    void* last = NULL;
    for (int i = 0; i < (BM_BUF_SIZE / 2); i++) {
        last = BuddyAllocator_malloc(b_alloc, 100);
    }
    DATA_MAX big_offset = b_alloc->min_bucket_size + 1;
    BuddyAllocator_free(b_alloc, (uint8_t*)last + big_offset);
}

void createError_BA_DoubleFree(BuddyAllocator *b_alloc) {
    void* test = BuddyAllocator_malloc(b_alloc, 100);
    BuddyAllocator_free(b_alloc, test);
    BuddyAllocator_free(b_alloc, test);
}
