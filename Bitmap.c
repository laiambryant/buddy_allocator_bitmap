#include "Bitmap.h"

// returns the number of bytes to store bits booleans
DATA_MAX BitMap_getBytes(DATA_MAX bits){
    return (bits>>3) + ((bits%8U)!=0);
}

// initializes a bitmap on an external array
void BitMap_init(BitMap *bit_map, DATA_MAX num_bits, uint8_t *buffer){
    assert(buffer!=NULL);
    bit_map->Buf = buffer;
    bit_map->num_bits = num_bits;
    bit_map->buffer_size = BitMap_getBytes(num_bits);
    bit_map->end_Buf = buffer+num_bits-1;
    for(uint32_t i = 0; i<num_bits; i++){
		BitMap_setBit(bit_map, i, FREE);
	}
}

// sets a the bit bit_num in the bitmap
// status= 0 or 1
void BitMap_setBit(BitMap *bit_map, DATA_MAX bit_num, Status status){
    DATA_MAX page = bit_num>>3;
    assert(page<bit_map->buffer_size);
    DATA_MAX offset =  bit_num %8;
    if(status==ALLOCATED) 
        bit_map->Buf[page] |= (1U<<offset);
    if(status==FREE)
        bit_map->Buf[page] &= ~(1U<<offset);
}

// inspects the status of the bit bit_num
uint8_t BitMap_bit(BitMap *bit_map, DATA_MAX bit_num){
    DATA_MAX page = bit_num>>3;
    assert(page<bit_map->buffer_size);
    DATA_MAX offset =  bit_num %8;
    return (bit_map->Buf[page] & (1U<<offset))!=0; 
}

void Bitmap_print(BitMap *bit_map, OUT_MODE out_mode){
    if(bit_map->Buf!=NULL){
        if(out_mode==F_WRITE){
            FILE* f = fopen("OUT/bitmap.txt", "w");
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fprintf(f, "Bitmap Metadata:\n ");
            fprintf(f, "%d bits\t%d bytes\n", bit_map->num_bits, bit_map->buffer_size);
            fprintf(f, "%p start\t%p end\n", bit_map->Buf, bit_map->end_Buf);
            fprintf(f, "Bitmap STATUS:\n ");
            for(DATA_MAX i=0; i<bit_map->num_bits; i++){
                fprintf(f, "%x", BitMap_bit(bit_map, i));  
            }
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fclose(f);
        }
        if(out_mode==STDOUT){
            fprintf(stdout, "\n----------------------------------------------------------------------------------------------\n");
            fprintf(stdout,"Bitmap Metadata:\n "); 
            fprintf(stdout,"%d bits\t%d bytes\n",bit_map->num_bits, bit_map->buffer_size);
            fprintf(stdout, "%p start\t%p end\n", bit_map->Buf, bit_map->end_Buf);
            fprintf(stdout,"Bitmap STATUS:\n ");
            for(DATA_MAX i=0; i<bit_map->num_bits; i++){
                fprintf(stdout, "%x", BitMap_bit(bit_map, i));
            }
            fprintf(stdout,"\n----------------------------------------------------------------------------------------------\n");

        }
        if(out_mode==F_CONCAT){
            FILE* f = fopen("OUT/bitmap.txt", "a");
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fprintf(f, "Bitmap Metadata:\n ");
            fprintf(f, "%d bits\t%d bytes\n",  bit_map->num_bits, bit_map->buffer_size);
            fprintf(f, "%p start\t%p end\n", bit_map->Buf, bit_map->end_Buf);
            fprintf(f, "Bitmap STATUS:\n ");
            for(DATA_MAX i=0; i<bit_map->num_bits; i++){
                fprintf(f, "%x", BitMap_bit(bit_map, i));
            }
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fclose(f);
        }
    }
}