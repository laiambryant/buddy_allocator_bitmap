#include "Bitmap.h"

// initializes a bitmap on an external array
BitMap* BitMap_init(uint8_t *buffer, DATA_MAX buf_size){

    //Finds addr for struct and for bitmap
    BitMap* bit_map = (BitMap*) buffer;
    uint8_t* bm_mem_start = ((uint8_t*)buffer)+sizeof(BitMap);

    assert(buffer!=NULL);
    
    //Initializes params
    bit_map->Buf = bm_mem_start;
    bit_map->num_bits = buf_size;
    bit_map->buffer_size = BitMap_getBytes(bit_map->num_bits);
    bit_map->end_Buf = buffer+bit_map->buffer_size;
    
    //sets all bits to 0
    for(DATA_MAX i = 0; i<buf_size; i++){
		BitMap_setBit(bit_map, i, FREE);
	}
    
    return bit_map;
}

// sets a the bit bit_num in the bitmap status 1 or 0, Returns 0 if unsuccessful 1 if successful
DATA_MAX BitMap_setBit(BitMap *bit_map, DATA_MAX bit_num, Status status){
    if(bit_num<0) return 0;
    DATA_MAX page = bit_num>>3;
    //assert(page<bit_map->buffer_size);
    DATA_MAX offset =  bit_num %8;
    if(status==ALLOCATED) 
        bit_map->Buf[page] |= (1U<<offset);
    if(status==FREE)
        bit_map->Buf[page] &= ~(1U<<offset);
    return 1;
}

// inspects the status of the bit bit_num
uint8_t BitMap_getBit(BitMap *bit_map, DATA_MAX bit_num){
    DATA_MAX page = bit_num>>3;
    if(page>bit_map->buffer_size){
        printf("[Page]: %d\t[Buffer size]: %d\t [Bit]: %d\n", page, bit_map->buffer_size, bit_num);
    }
    //assert(page<bit_map->buffer_size);
    DATA_MAX offset =  bit_num %8;
    return (bit_map->Buf[page] & (1U<<offset))!=0; 
}

void Bitmap_print(BitMap *bit_map, OUT_MODE out_mode){
    if(bit_map->Buf!=NULL){
        if(out_mode==F_WRITE){
            FILE* f = fopen("OUT/Logs/bitmap.txt", "w");
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fprintf(f, "Bitmap Metadata:\n");
            fprintf(f, "%d bits\t%d bytes\n", bit_map->num_bits, bit_map->buffer_size);
            fprintf(f, "%p start\t%p end\n", bit_map->Buf, bit_map->end_Buf);
            fprintf(f, "Bitmap STATUS:\n");
            for(int i=0; i<bit_map->num_bits; i++){
                fprintf(f, "%x", BitMap_getBit(bit_map, i));  
            }
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fclose(f);
        }
        if(out_mode==STDOUT){
            fprintf(stdout, "\n----------------------------------------------------------------------------------------------\n");
            fprintf(stdout,"Bitmap Metadata:\n"); 
            fprintf(stdout,"%d bits\t%d bytes\n",bit_map->num_bits, bit_map->buffer_size);
            fprintf(stdout, "%p start\t%p end\n", bit_map->Buf, bit_map->end_Buf);
            fprintf(stdout,"Bitmap STATUS:\n");
            for(int i=0; i<bit_map->num_bits; i++){
                fprintf(stdout, "%x", BitMap_getBit(bit_map, i));
            }
            fprintf(stdout,"\n----------------------------------------------------------------------------------------------\n");

        }
        if(out_mode==F_CONCAT){
            FILE* f = fopen("OUT/Logs/bitmap.txt", "a");
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fprintf(f, "Bitmap Metadata:\n");
            fprintf(f, "%d bits\t%d bytes\n",  bit_map->num_bits, bit_map->buffer_size);
            fprintf(f, "%p start\t%p end\n", bit_map->Buf, bit_map->end_Buf);
            fprintf(f, "Bitmap STATUS:\n");
            for(int i=0; i<bit_map->num_bits; i++){
                fprintf(f, "%x", BitMap_getBit(bit_map, i));
            }
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fclose(f);
        }
    }
}

// returns the number of bytes to store bits booleans
DATA_MAX BitMap_getBytes(DATA_MAX bits){
    return ((bits>>3) + ((bits%8)!=0));
}