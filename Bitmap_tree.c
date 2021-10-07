#include "Bitmap_tree.h"

DATA_MAX tree_level(BitMap_tree* tree, DATA_MAX idx){
    DATA_MAX ret = floor(log2(idx)); //2^level=node_idx => floor(log_2(node_idx)) = level
    if(ret>tree->levels)return tree->levels-1;
    if(ret>=0) return ret;
    else return 0;
}
DATA_MAX tree_first_node_level(BitMap_tree* tree,DATA_MAX idx){
    return ((0x01<<tree_level(tree, idx)));
}
DATA_MAX tree_first_free_node_level(BitMap_tree* tree,DATA_MAX level){
    if(level == 0 && tree_getBit(tree, 0)==ALLOCATED) return -1;
    DATA_MAX start = pow(2, level); DATA_MAX end = pow(2, level+1);
    for(DATA_MAX i=start;i<end;i++){
        if(tree_getBit(tree, i)==FREE) return i;
    }
    return -1;
}
DATA_MAX tree_node_level_offset(BitMap_tree* tree, DATA_MAX idx){
    return tree_first_node_level(tree, tree_level(tree, idx))-idx;
}
DATA_MAX tree_getbuddy(DATA_MAX idx){
    return 
        (idx & 0x0001)?(idx-1):(idx);
}
DATA_MAX tree_getparent(DATA_MAX idx){
    return (uint16_t)idx/2;
}
DATA_MAX tree_buddiesOnLevel(BitMap_tree *tree, DATA_MAX level){
    if(level == 0){
        if (tree_getBit(tree, 0)) return 1;
        else return 0;
    }
    DATA_MAX start_idx = pow(2, level);
    DATA_MAX end_idx = pow(2, level+1);
    DATA_MAX ret = 0;
    for(int i = start_idx; i<end_idx; i++){
        if(tree_getBit(tree, i)==ALLOCATED)ret++;
    }
    return ret;
}
void tree_print(BitMap_tree *tree, OUT_MODE out_mode){
    if(out_mode==F_WRITE){
        FILE* f = fopen("OUT/Logs/bitmap_tree.txt", "w");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(f, "Tree Metadata:\n");
        fprintf(f, "%d bits\t%d bytes\n", tree->BitMap->num_bits, tree->BitMap->buffer_size);
        fprintf(f, "%p start\t%p end\n", tree->BitMap->Buf, tree->BitMap->end_Buf);
        for (int i = 0; i < tree->levels; i++){
            fprintf(f,"Level %d: %d buddies\t",i, tree_buddiesOnLevel(tree, i));
            fprintf(f,"LVL first idx = : %.f, last idx: %.f \t", pow(2, i), pow(2,i+1)-1);
            fprintf(f,"First_free: %d\n",tree_first_free_node_level(tree, i));  
        }
        fprintf(f, "\n");
        fprintf(f, "Bitmap STATUS:\n");
        for (int i = 0; i < tree->levels; i++){
		    for (int j = 0; j < pow(2,i);j++){
			    fprintf(f, "%x",tree_getBit(tree,pow(2,i)+j));
		    }
		fprintf(f,"\n");
	    }
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fclose(f);
    }
    if(out_mode==STDOUT){
        fprintf(stdout, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(stdout,"Tree Metadata:\n"); 
        fprintf(stdout,"%d bits\t%d bytes\n",tree->BitMap->num_bits, tree->BitMap->buffer_size);
        fprintf(stdout, "%p start\t%p end\n", tree->BitMap->Buf, tree->BitMap->end_Buf);
        for (int i = 0; i < tree->levels; i++){
            fprintf(stdout,"Level %d: %d buddies\t",i, tree_buddiesOnLevel(tree, i));
            fprintf(stdout,"LVL first idx = : %.f, last idx: %.f \t", pow(2, i), pow(2,i+1)-1);
            fprintf(stdout,"First_free: %d\n",tree_first_free_node_level(tree, i));
        }
        fprintf(stdout,"\n");
        fprintf(stdout,"Bitmap STATUS:\n");
        for (int i = 0; i < tree->levels; i++){
		    for (int j = 0; j < pow(2,i);j++){
			    fprintf(stdout, "%x",tree_getBit(tree,pow(2,i)+j));
		    }
		fprintf(stdout,"\n");
	    }
        fprintf(stdout,"\n----------------------------------------------------------------------------------------------\n");
    }
    if(out_mode==F_CONCAT){
        FILE* f = fopen("OUT/Logs/bitmap_tree.txt", "a");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(f, "Tree Metadata:\n");
        fprintf(f, "%d bits\t%d bytes\n",  tree->BitMap->num_bits, tree->BitMap->buffer_size);
        fprintf(f, "%p start\t%p end\n", tree->BitMap->Buf, tree->BitMap->end_Buf);
        for (int i = 0; i < tree->levels; i++){
            fprintf(f,"Level %d: %d buddies\t",i, tree_buddiesOnLevel(tree, i));
            fprintf(f,"LVL first idx = : %.f, last idx: %.f \t", pow(2, i), pow(2,i+1)-1);
            fprintf(f,"First_free: %d\n",tree_first_free_node_level(tree, i));
        }
        fprintf(f,"\n");
        fprintf(f, "Bitmap STATUS:\n");
        for (int i = 0; i < tree->levels; i++){
		    for (int j = 0; j < pow(2,i);j++){
			    fprintf(f,"%x",tree_getBit(tree,(pow(2,i)+j)));
		    }
		fprintf(f,"\n");
	    }
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fclose(f);
    }
}

DATA_MAX tree_nodes(DATA_MAX levels){
    return (pow(2, levels)+1);
}
DATA_MAX tree_leafs(DATA_MAX levels){
    return (pow(2, levels));
}

DATA_MAX tree_free_buddies_on_level(BitMap_tree* tree, DATA_MAX level){
    DATA_MAX start_idx = pow(2, level);
    DATA_MAX end_idx = pow(2, level+1);
    for(int i = start_idx; i<end_idx; i++){
        if(tree_getBit(tree, i)==FREE) return 1;
    }
    return 0;
}

DATA_MAX tree_balloc_getIdx(BitMap_tree* tree, DATA_MAX level){
    DATA_MAX start_idx = pow(2, level);
    DATA_MAX end_idx = pow(2, level+1)-1;
    for(int i = start_idx; i<end_idx; i++){
        if(tree_getBit(tree, i)==FREE){
            tree_setParents(tree, level, i, ALLOCATED);
            tree_setChildren(tree, i, ALLOCATED);
            tree_setBit(tree, i, ALLOCATED);
            return i;
        }
    }
    return 0;
}

void tree_setParents(BitMap_tree* tree, DATA_MAX level, DATA_MAX idx, Status status){
    DATA_MAX parent_idx = tree_getparent(idx);
    while(level>0){
        tree_setBit(tree, parent_idx, status);
        parent_idx = tree_getparent(parent_idx);
        level--;
    }
}
void tree_setChildren(BitMap_tree* tree, DATA_MAX idx, Status status){
    DATA_MAX level = tree_level(tree, idx);
    DATA_MAX left_child = idx << 1;
    DATA_MAX right_child = left_child +1;
    tree_setChildren_internal(tree, left_child, right_child, status);
}

void tree_setChildren_internal(BitMap_tree* tree, DATA_MAX l_child, DATA_MAX r_child, Status status){
    
    if(l_child >(tree->total_nodes) || r_child>(tree->total_nodes)) return;
    else{
        tree_setBit(tree, l_child, status);
        tree_setBit(tree, r_child, status);

        DATA_MAX ll_child = l_child<<1; DATA_MAX lr_child = ll_child +1;
        DATA_MAX rl_child = r_child<<1; DATA_MAX rr_child = rl_child +1;

        tree_setChildren_internal(tree, ll_child, lr_child, status);
        tree_setChildren_internal(tree, rl_child, rr_child, status);
        
    }

}

void tree_setBit(BitMap_tree *tree, DATA_MAX bit_num, Status status){
    printf("Setting Bit[%d]\n", bit_num);
    BitMap_setBit(tree->BitMap, bit_num, status);
}

DATA_MAX tree_getBit(BitMap_tree *tree, DATA_MAX bit_num){
    return BitMap_bit(tree->BitMap, bit_num);
}