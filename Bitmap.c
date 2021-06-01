#include "Bitmap.h"
#include <stdio.h>
#include <assert.h>

// returns the number of bytes to store bits booleans
DATA_MAX BitMap_getBytes(DATA_MAX bits){
    return (bits/8) + ((bits%8)!=0);
}

// initializes a bitmap on an external array
void BitMap_init(BitMap *bit_map, DATA_MAX num_bits, uint8_t *buffer){
    assert(buffer!=NULL);
    bit_map->Buf = buffer;
    bit_map->num_bits = num_bits;
    bit_map->buffer_size = BitMap_getBytes(bit_map->num_bits);
}

// sets a the bit bit_num in the bitmap
// status= 0 or 1
void BitMap_setBit(BitMap *bit_map, DATA_MAX bit_num, Status status){
    DATA_MAX byte_num = bit_num>>3;
    assert(byte_num<bit_map->buffer_size);
    DATA_MAX bit_in_byte = byte_num & 0x03;
    if(status==ALLOCATED) 
        bit_map->Buf[byte_num] |= (1<<bit_in_byte);
    if(status==FREE)
        bit_map->Buf[byte_num] &= ~(1<<bit_in_byte);
}

// inspects the status of the bit bit_num
Status BitMap_bit(BitMap *bit_map, DATA_MAX bit_num){
    DATA_MAX byte_num = bit_num>>3;
    assert(byte_num<bit_map->buffer_size);
    DATA_MAX bit_in_byte = byte_num & 0x03;
    if ((bit_map->Buf[byte_num]&(1<<bit_in_byte))!=0)
        return ALLOCATED;
    else return FREE;
}

void Bitmap_print(BitMap *bit_map, OUT_MODE out_mode){
    if(bit_map->Buf!=NULL){
        if(out_mode==F_WRITE){
            FILE* f = fopen("OUT/bitmap.txt", "w");
            fprintf(f, "Bitmap Metadata:\n ");
            fprintf(f, "%d bits\t%d bytes\n", bit_map->num_bits, bit_map->buffer_size);
            fprintf(f, "Bitmap STATUS:\n ");
            for(DATA_MAX i=0; i<bit_map->num_bits; i++){
                fprintf(f, "%d", BitMap_bit(bit_map, i));
            }
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fclose(f);
        }
        if(out_mode==STDOUT){
            printf("Bitmap Metadata:\n "); 
            printf("%d bits\t%d bytes\n",bit_map->num_bits, bit_map->buffer_size);
            printf("Bitmap STATUS:\n ");
            for(DATA_MAX i=0; i<bit_map->num_bits; i++){
                printf("%d", BitMap_bit(bit_map, i));
            }
            printf("\n----------------------------------------------------------------------------------------------\n");

        }
        if(out_mode==F_CONCAT){
            FILE* f = fopen("OUT/bitmap.txt", "a");
            fprintf(f, "Bitmap Metadata:\n ");
            fprintf(f, "%d bits\t%d bytes\n",  bit_map->num_bits, bit_map->buffer_size);
            fprintf(f, "Bitmap STATUS:\n ");
            for(DATA_MAX i=0; i<bit_map->num_bits; i++){
                fprintf(f, "%d", BitMap_bit(bit_map, i));
            }
            fprintf(f, "\n----------------------------------------------------------------------------------------------\n");
            fclose(f);
        }
    }
}