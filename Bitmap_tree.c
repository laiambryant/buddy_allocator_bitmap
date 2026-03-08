#include "Bitmap_tree.h"

BitMap_tree* BitMap_tree_init(uint8_t* buffer, DATA_MAX buffer_size, DATA_MAX levels) {
    BitMap_tree* tree = (BitMap_tree*)buffer;
    uint8_t* bitmap_struct = buffer + sizeof(BitMap_tree);
    tree->BitMap = (BitMap*)bitmap_struct;
    tree->levels = levels;
    tree->total_nodes = tree_nodes(levels);
    tree->leaf_num = tree_leafs(levels);
    DATA_MAX bm_size = buffer_size - (sizeof(BitMap_tree) + sizeof(BitMap));
    BitMap_init(bitmap_struct, bm_size * 8);
    return tree;
}

DATA_MAX tree_level(BitMap_tree* tree, DATA_MAX idx) {
    DATA_MAX ret = (int)floor(log2(idx));
    if (ret > tree->levels) return tree->levels - 1;
    if (ret >= 0) return ret;
    return 0;
}

DATA_MAX tree_first_node_level(BitMap_tree* tree, DATA_MAX idx) {
    return (0x01 << tree_level(tree, idx));
}

DATA_MAX tree_first_free_node_level(BitMap_tree* tree, DATA_MAX level) {
    DATA_MAX start = 1 << level;
    DATA_MAX end = 1 << (level + 1);
    for (DATA_MAX i = start; i < end; i++) {
        if (tree_getBit(tree, i) == FREE) return i;
    }
    return -1;
}

DATA_MAX tree_node_level_offset(BitMap_tree* tree, DATA_MAX idx) {
    return tree_first_node_level(tree, tree_level(tree, idx)) - idx;
}

DATA_MAX tree_getbuddy(DATA_MAX idx) {
    return (idx & 0x0001) ? (idx - 1) : (idx + 1);
}

DATA_MAX tree_getparent(DATA_MAX idx) {
    return (uint16_t)idx / 2;
}

DATA_MAX tree_buddiesOnLevel(BitMap_tree *tree, DATA_MAX level) {
    DATA_MAX start_idx = 1 << level;
    DATA_MAX end_idx = 1 << (level + 1);
    DATA_MAX ret = 0;
    for (int i = start_idx; i < end_idx; i++) {
        if (tree_getBit(tree, i) == ALLOCATED) ret++;
    }
    return ret;
}

void tree_print(BitMap_tree *tree, FILE *out) {
    fprintf(out, "\n----------------------------------------------------------------------------------------------\n");
    fprintf(out, "Tree Metadata:\n");
    fprintf(out, "%d bits\t%d bytes\n", tree->BitMap->num_bits, tree->BitMap->buffer_size);
    fprintf(out, "%p start\t%p end\n", (void*)tree->BitMap->Buf, (void*)tree->BitMap->end_Buf);
    for (int i = 0; i < tree->levels; i++) {
        fprintf(out, "Level %d: %d buddies\t", i, tree_buddiesOnLevel(tree, i));
        fprintf(out, "LVL first idx = : %d, last idx: %d \t", 1 << i, (1 << (i + 1)) - 1);
        fprintf(out, "First_free: %d\n", tree_first_free_node_level(tree, i));
    }
    fprintf(out, "\n");
    fprintf(out, "Bitmap STATUS:\n");
    for (int i = 0; i < tree->levels; i++) {
        for (int j = 0; j < (1 << i); j++) {
            fprintf(out, "%x", tree_getBit(tree, (1 << i) + j));
        }
        fprintf(out, "\n");
    }
    fprintf(out, "\n----------------------------------------------------------------------------------------------\n");
}

DATA_MAX tree_nodes(DATA_MAX levels) {
    return ((1 << levels) + 1);
}

DATA_MAX tree_leafs(DATA_MAX levels) {
    return (1 << levels);
}

DATA_MAX tree_free_buddies_on_level(BitMap_tree* tree, DATA_MAX level) {
    DATA_MAX start_idx = 1 << level;
    DATA_MAX end_idx = 1 << (level + 1);
    for (int i = start_idx; i < end_idx; i++) {
        if (tree_getBit(tree, i) == FREE) return 1;
    }
    return 0;
}

DATA_MAX tree_balloc_getIdx(BitMap_tree* tree, DATA_MAX level) {
    DATA_MAX start_idx = 1 << level;
    DATA_MAX end_idx = 1 << (level + 1);
    for (int i = start_idx; i < end_idx; i++) {
        if (tree_getBit(tree, i) == FREE) {
            tree_setParents(tree, i, level, ALLOCATED);
            tree_setChildren(tree, i, ALLOCATED);
            tree_setBit(tree, i, ALLOCATED);
            return i;
        }
    }
    return 0;
}

void tree_setParents(BitMap_tree* tree, DATA_MAX idx, DATA_MAX level, Status status) {
    if (status == FREE) {
        DATA_MAX parent_idx = tree_getparent(idx);
        while (level > 0) {
            if (tree_getBit(tree, tree_getbuddy(idx)) == FREE) {
                tree_setBit(tree, parent_idx, status);
                parent_idx = tree_getparent(parent_idx);
                level--;
            } else return;
        }
    }
    if (status == ALLOCATED) {
        DATA_MAX parent_idx = tree_getparent(idx);
        while (level > 0) {
            tree_setBit(tree, parent_idx, status);
            parent_idx = tree_getparent(parent_idx);
            level--;
        }
    }
}

void tree_setChildren(BitMap_tree* tree, DATA_MAX idx, Status status) {
    DATA_MAX left_child = idx << 1;
    DATA_MAX right_child = left_child + 1;
    tree_setChildren_internal(tree, left_child, right_child, status);
}

void tree_setChildren_internal(BitMap_tree* tree, DATA_MAX l_child, DATA_MAX r_child, Status status) {
    if (l_child > (tree->total_nodes) || r_child > (tree->total_nodes)) return;
    tree_setBit(tree, l_child, status);
    tree_setBit(tree, r_child, status);
    DATA_MAX ll_child = l_child << 1;
    DATA_MAX lr_child = ll_child + 1;
    DATA_MAX rl_child = r_child << 1;
    DATA_MAX rr_child = rl_child + 1;
    tree_setChildren_internal(tree, ll_child, lr_child, status);
    tree_setChildren_internal(tree, rl_child, rr_child, status);
}

void tree_setBit(BitMap_tree *tree, DATA_MAX bit_num, Status status) {
    BitMap_setBit(tree->BitMap, bit_num, status);
}

DATA_MAX tree_getBit(BitMap_tree *tree, DATA_MAX bit_num) {
    return BitMap_getBit(tree->BitMap, bit_num);
}
