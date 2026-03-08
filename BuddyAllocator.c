#include "BuddyAllocator.h"

static const char* BuddyAllocator_strerrors[] = {
    "Success",
    "NotEnoughMemory",
    "UnalignedFree",
    "OutOfRange",
    "DoubleFree",
    0
};

BuddyAllocator* BuddyAllocator_init(
    uint8_t* ba_buffer,
    DATA_MAX buffer_size,
    DATA_MAX num_levels) {
    assert(num_levels < MAX_LEVELS);
    BuddyAllocator* b_alloc = (BuddyAllocator*)ba_buffer;
    uint8_t* tree_buff = ((uint8_t*)b_alloc) + sizeof(BuddyAllocator);
    DATA_MAX tree_mem_size = sizeof(BitMap) + sizeof(BitMap_tree) + (1 << num_levels);
    BitMap_tree* tree = BitMap_tree_init(tree_buff, tree_mem_size, num_levels);
    const DATA_MAX balloc_mem_size = buffer_size - (sizeof(BuddyAllocator) + tree_mem_size);
    b_alloc->num_levels = num_levels;
    b_alloc->num_items = 1 << num_levels;
    b_alloc->memory = tree_buff + tree_mem_size;
    b_alloc->tree = tree;
    b_alloc->min_bucket_size = balloc_mem_size >> (num_levels - 1);
    b_alloc->buffer_size = balloc_mem_size;
    b_alloc->leaf_num = b_alloc->buffer_size / b_alloc->min_bucket_size;
    b_alloc->user_mem = b_alloc->min_bucket_size * b_alloc->leaf_num;
    if (DEBUG) BuddyAllocator_printMetadata(b_alloc, stdout);
    return b_alloc;
}

void* BuddyAllocator_getBuddy(BuddyAllocator* b_alloc, DATA_MAX level) {
    assert(level >= 0);
    assert(level <= b_alloc->num_levels);
    if (!tree_free_buddies_on_level(b_alloc->tree, level))
        return NULL;
    DATA_MAX idx = tree_balloc_getIdx(b_alloc->tree, level);
    if (idx == 0) return NULL;
    DATA_MAX offset = b_alloc->min_bucket_size * (b_alloc->num_levels - level) * (idx - tree_first_node_level(b_alloc->tree, idx));
    void* ret = (void*)(b_alloc->memory + offset);
    DATA_MAX* tmp = ret;
    *tmp = idx;
    ret = (uint8_t*)ret + sizeof(DATA_MAX);
    if (DEBUG) {
        fprintf(stdout, "%d * (%d - %d) * (%d - %d) = %d\n", b_alloc->min_bucket_size, b_alloc->num_levels, level, idx, tree_first_node_level(b_alloc->tree, idx), offset);
        fprintf(stdout, "[Address]:%p \t[Offset]: %d, [Idx]: %d\n\n", ret, offset, *tmp);
        tree_print(b_alloc->tree, stdout);
    }
    return ret;
}

BuddyAllocatorResult BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, void* item) {
    DATA_MAX offset = ((uint8_t*)item - sizeof(DATA_MAX)) - alloc->memory;
    DATA_MAX max_offset = alloc->user_mem;
    if (offset > max_offset)
        return BA_OutOfRange;
    DATA_MAX* idx_ptr = (DATA_MAX*)((uint8_t*)item - sizeof(DATA_MAX));
    DATA_MAX idx = *idx_ptr;
    if (DEBUG) {
        fprintf(stdout, "Freeing memory: @%p\t", (void*)idx_ptr);
        fprintf(stdout, "Item idx: [%d], num_items[%d], offset[%d]\n", idx, alloc->num_items, offset);
    }
    if (offset % alloc->min_bucket_size)
        return BA_UnalignedFree;
    if (tree_getBit(alloc->tree, idx) == FREE)
        return BA_DoubleFree;
    tree_setBit(alloc->tree, idx, FREE);
    tree_setParents(alloc->tree, idx, tree_level(alloc->tree, idx), FREE);
    tree_setChildren(alloc->tree, idx, FREE);
    if (DEBUG) tree_print(alloc->tree, stdout);
    return BA_Success;
}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, DATA_MAX size) {
    DATA_MAX level = tree_level(alloc->tree, alloc->user_mem / (size + sizeof(DATA_MAX)));
    if (level > alloc->num_levels) level = alloc->num_levels;
    if (DEBUG) fprintf(stdout, "Requested Buddy of %d bytes, level %d\n", size, level);
    void* buddy_ptr = BuddyAllocator_getBuddy(alloc, level);
    if (buddy_ptr == NULL) {
        fprintf(stderr, "Malloc: %s\n", BuddyAllocator_strerror(BA_NotEnoughMemory));
        assert(buddy_ptr != NULL);
    }
    return buddy_ptr;
}

void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
    if (mem == NULL) {
        fprintf(stderr, "[FREE][Error, NULL pointer]: exiting...\n");
        assert(0);
    }
    BuddyAllocatorResult res = BuddyAllocator_releaseBuddy(alloc, mem);
    if (DEBUG) fprintf(stdout, "Result: %s\n", BuddyAllocator_strerror(res));
}

void BuddyAllocator_printMetadata(BuddyAllocator* b_alloc, FILE *out) {
    fprintf(out, "\n----------------------------------------------------------------------------------------------\n");
    fprintf(out, "\tAlloc Address: %p\n", (void*)b_alloc);
    fprintf(out, "\tTree Address: %p\n", (void*)b_alloc->tree);
    fprintf(out, "\tBitmap Address: %p\n", (void*)b_alloc->tree->BitMap);
    fprintf(out, "\tBuddy Allocator levels: %d\n", b_alloc->num_levels);
    fprintf(out, "\tBuddy Allocator leafs: %d\n", b_alloc->leaf_num);
    fprintf(out, "\tBuddy Allocator min bucket size: %d\n", b_alloc->min_bucket_size);
    fprintf(out, "\tMem alloc size: %d\n", b_alloc->buffer_size);
    fprintf(out, "\tUser mem: %d\n", b_alloc->user_mem);
    fprintf(out, "\tMem address start: %p\n", (void*)b_alloc->memory);
    fprintf(out, "\tMem address end: %p\n", (void*)(b_alloc->memory + b_alloc->buffer_size));
    fprintf(out, "\n----------------------------------------------------------------------------------------------\n");
}

const char* BuddyAllocator_strerror(BuddyAllocatorResult result) {
    return BuddyAllocator_strerrors[result];
}
