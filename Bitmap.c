#include "Bitmap.h"

// returns the number of bytes to store bits booleans
int BitMap_getBytes(int bits){
    return (bits/8) + ((bits%8)!=0);
}

// initializes a bitmap on an external array
void BitMap_init(BitMap *bit_map, int num_bits, uint8_t *buffer){
    assert(buffer!=NULL);
    bit_map->Buf = buffer;
    bit_map->num_bits = num_bits;
    bit_map->buffer_size = BitMap_getBytes(num_bits);
    for(int i = 0; i<num_bits; i++){
		BitMap_setBit(bit_map, i, FREE);
	}
    bit_map->end_Buf = buffer+num_bits-1;
}

// sets a the bit bit_num in the bitmap
// status= 0 or 1
void BitMap_setBit(BitMap *bit_map, int bit_num, Status status){
    int byte_num = bit_num>>3;
    assert(byte_num<bit_map->buffer_size);
    int bit_in_byte = byte_num & 0x03;
    if(status==ALLOCATED) 
        bit_map->Buf[byte_num] |= (1<<bit_in_byte);
    if(status==FREE)
        bit_map->Buf[byte_num] &= ~(1<<bit_in_byte);
}

// inspects the status of the bit bit_num
Status BitMap_bit(BitMap *bit_map, int bit_num){
    int byte_num = bit_num>>3;
    assert(byte_num<bit_map->buffer_size);
    int bit_in_byte = byte_num & 0x03;
    if ((bit_map->Buf[byte_num]&(1<<bit_in_byte))!=0)
        return ALLOCATED;
    else return FREE;
}

void Bitmap_print(BitMap *bit_map, OUT_MODE out_mode){
    if(bit_map->Buf!=NULL){
        if(out_mode==F_WRITE){
            FILE* f = fopen("OUT/bitmap.txt", "w");
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fprintf(f, "Bitmap Metadata:\n ");
            fprintf(f, "%ld bits\t%ld bytes\n", bit_map->num_bits, bit_map->buffer_size);
            fprintf(f, "%p start\t%p end\n", bit_map->Buf, bit_map->end_Buf);
            fprintf(f, "Bitmap STATUS:\n ");
            for(int i=0; i<bit_map->num_bits; i++){
                fprintf(f, "%d", BitMap_bit(bit_map, i));
            }
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fclose(f);
        }
        if(out_mode==STDOUT){
            fprintf(stdout, "\n----------------------------------------------------------------------------------------------\n");
            fprintf(stdout,"Bitmap Metadata:\n "); 
            fprintf(stdout,"%ld bits\t%ld bytes\n",bit_map->num_bits, bit_map->buffer_size);
            fprintf(stdout, "%p start\t%p end\n", bit_map->Buf, bit_map->end_Buf);
            fprintf(stdout,"Bitmap STATUS:\n ");
            for(int i=0; i<bit_map->num_bits; i++){
                fprintf(stdout,"%d", BitMap_bit(bit_map, i));
            }
            fprintf(stdout,"\n----------------------------------------------------------------------------------------------\n");

        }
        if(out_mode==F_CONCAT){
            FILE* f = fopen("OUT/bitmap.txt", "a");
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fprintf(f, "Bitmap Metadata:\n ");
            fprintf(f, "%ld bits\t%ld bytes\n",  bit_map->num_bits, bit_map->buffer_size);
            fprintf(f, "%p start\t%p end\n", bit_map->Buf, bit_map->end_Buf);
            fprintf(f, "Bitmap STATUS:\n ");
            for(int i=0; i<bit_map->num_bits; i++){
                fprintf(f, "%d", BitMap_bit(bit_map, i));
            }
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fclose(f);
        }
    }
}