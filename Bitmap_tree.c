#include "Bitmap_tree.h"

DATA_MAX tree_get_idx(Buddy_item *bud){
    if(bud){
        return bud->idx;
    }
    else return -1;
}

DATA_MAX tree_level(DATA_MAX idx){
    return (DATA_MAX) floor(log2(idx)); //2^level=node_idx => floor(log_2(node_idx)) = level
}
DATA_MAX tree_first_node_level(DATA_MAX idx){
    return 0x0001<<tree_level(idx);
}
DATA_MAX tree_node_level_offset(DATA_MAX idx){
    return idx-tree_first_node_level(tree_level(idx));
}
DATA_MAX tree_getbuddy(DATA_MAX idx){
    return 
        (idx & 0x0001)?(idx-1):(idx);
}
DATA_MAX tree_getparent(DATA_MAX idx){
    return (uint16_t)idx/2;
}

void tree_print(BitMap_tree *tree, OUT_MODE out_mode){
    if(out_mode==F_WRITE){
        FILE* f = fopen("OUT/bitmap.txt", "w");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(f, "Bitmap Metadata:\n ");
        fprintf(f, "%d bits\t%d bytes\n", tree->BitMap->num_bits, tree->BitMap->buffer_size);
        fprintf(f, "%p start\t%p end\n", tree->BitMap->Buf, tree->BitMap->end_Buf);
        fprintf(f, "Bitmap STATUS:\n ");
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
        fprintf(stdout,"Bitmap Metadata:\n "); 
        fprintf(stdout,"%d bits\t%d bytes\n",tree->BitMap->num_bits, tree->BitMap->buffer_size);
        fprintf(stdout, "%p start\t%p end\n", tree->BitMap->Buf, tree->BitMap->end_Buf);
        fprintf(stdout,"Bitmap STATUS:\n ");
        for (int i = 0; i < tree->levels; i++){
		    for (int j = 0; j < pow(2,i);j++){
			    fprintf(stdout, "%x",BitMap_bit(tree->BitMap,pow(2,i)+j));
		    }
		fprintf(stdout,"\n");
	    }
        fprintf(stdout,"\n----------------------------------------------------------------------------------------------\n");
    }
    if(out_mode==F_CONCAT){
        FILE* f = fopen("OUT/bitmap.txt", "a");
        fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
        fprintf(f, "Bitmap Metadata:\n ");
        fprintf(f, "%d bits\t%d bytes\n",  tree->BitMap->num_bits, tree->BitMap->buffer_size);
        fprintf(f, "%p start\t%p end\n", tree->BitMap->Buf, tree->BitMap->end_Buf);
        fprintf(f, "Bitmap STATUS:\n ");
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

