#include "Bitmap_tree.h"

DATA_MAX tree_get_idx(Buddy_item *bud);

DATA_MAX tree_level(BitMap_tree* tree, DATA_MAX idx){
    DATA_MAX ret = floor(log2(idx)); //2^level=node_idx => floor(log_2(node_idx)) = level
    if(ret>tree->levels)return tree->levels;
    if(ret>=0) return ret;
    else return 0;
}
DATA_MAX tree_first_node_level(BitMap_tree* tree,DATA_MAX idx){
    return 0x01<<tree_level(tree, idx);
}
DATA_MAX tree_first_free_node_level(BitMap_tree* tree,DATA_MAX level){
    if(level == 0) {
        if(BitMap_bit(tree->BitMap,0)==ALLOCATED)return -1;
        else return 0;
    }
    DATA_MAX start = pow(2, level); DATA_MAX end = pow(2, level+1)-1;
    //printf("[START]: %d \t[END]: %d\n", start, end);
    for(DATA_MAX i=start;i<end;i++){
        if(BitMap_bit(tree->BitMap, i)==FREE) return i;
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
        if (BitMap_bit(tree->BitMap, 0)) return 1;
        else return 0;
    }
    if(level == 1){
        if (BitMap_bit(tree->BitMap, 1)==ALLOCATED && BitMap_bit(tree->BitMap, 2)==ALLOCATED) return 2;
        if (BitMap_bit(tree->BitMap, 1)==ALLOCATED || BitMap_bit(tree->BitMap, 2)==ALLOCATED) return 1;
        else return 0;
    }
    DATA_MAX start_idx = pow(2, level)-1;
    DATA_MAX end_idx = pow(2, level+1);
    DATA_MAX ret = 0;
    for(int i = start_idx; i<end_idx; i++){
        if(BitMap_bit(tree->BitMap, i)==ALLOCATED)ret++;
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
			    fprintf(f, "%x",BitMap_bit(tree->BitMap,pow(2,i)+j));
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
			    fprintf(stdout, "%x",BitMap_bit(tree->BitMap,pow(2,i)+j));
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
			    fprintf(f,"%x",BitMap_bit(tree->BitMap,(pow(2,i)+j)-1));
		    }
		fprintf(f,"\n");
	    }
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fclose(f);
    }
}
DATA_MAX tree_nodes(DATA_MAX levels){
    return (pow(2, levels)+1)-1;
}
DATA_MAX tree_leafs(DATA_MAX levels){
    return (pow(2, levels));
}

BitMap_tree* BitMap_tree_init(
    PoolAllocator* p_alloc, 
    DATA_MAX buf_size, 
    uint8_t *buffer,
    DATA_MAX levels){
    
    PoolAllocatorResult res =  PoolAllocator_init(
        p_alloc, sizeof(BitMap)+sizeof(BitMap_tree), 1, buffer, buf_size
        );

    if(DEBUG) {
        FILE* f = fopen("OUT/Logs/log.txt", "a");          
        fprintf(f, "[Bitmap]: %s\n",PoolAllocator_strerror(res));
        fclose(f); 
    }
    
    BitMap_tree* tree = (BitMap_tree*) PoolAllocator_getBlock(p_alloc);
    
    tree->BitMap->Buf = buffer+sizeof(BitMap)+sizeof(BitMap_tree);
    tree->BitMap->num_bits = buf_size;
    tree->BitMap->buffer_size = BitMap_getBytes(buf_size);
    tree->BitMap->end_Buf = buffer+buf_size;
    tree->leaf_num = tree_leafs(levels);
    tree->levels = levels;
    tree->total_nodes = tree_nodes(levels);
    
    return tree;
}