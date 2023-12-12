#include "img-trnsfrt.h"
#include "upd_rasp.h"
#include <stdio.h>
#include <string.h>
int add_partial_img(seq_img* img, partial_img * part_img){
    uint16_t seq_num=part_img->seq_num&(0x7FFFF);
    if (seq_num*part_img->len>(part_img->x*part_img->y)){
        perror("WARNING : Trying to write after the end of the image!!\nIgnoring the current packet");
        return ADD_PARTIAL_ERROR;
    }
    int index = seq_num*part_img->len;
    memcpy(img->img+index ,part_img->img_part,part_img->len );
    if (seq_num==part_img->seq_num){
        return ADD_PARTIAL_END;
    }
    return ADD_PARTIAL_OK;
}

char ** to_udp_buffer(char * img_buffer,int buffer_len,int x,int y, time_t timestamp, int packet_size,
                        int * udp_buf_cout, int * udp_buf_len){
    int udp_packet_size =(1+8 +2+2+2+2) +packet_size;
    int packet_count=  buffer_len/udp_packet_size;
    if (buffer_len%udp_packet_size!=0){
        packet_count+=1;
    }
   
    char ** part_imgs= malloc(packet_count*sizeof(char*));
    *udp_buf_cout=packet_count;
    *udp_buf_len=(HEADER_LEN_ANY+HEADER_LEN_IMG+packet_size);
    
    for (int i=0;i<packet_count;i++){
        part_imgs[i]=malloc(HEADER_LEN_ANY+HEADER_LEN_IMG+packet_size);
        int cur_buf_index=0;
        if (!part_imgs[i]){
            perror("ERROR : MALLOC ERROR : @ to_udp_buffer");
            exit(1);
        }
        part_imgs[i][cur_buf_index]=TYPE_IMG;
        cur_buf_index+=1;
        memcpy(part_imgs[i]+cur_buf_index,&timestamp,8);
        cur_buf_index+=8;
        memcpy(part_imgs[i]+cur_buf_index,(uint16_t*)&x,2);
        cur_buf_index+=2;
        memcpy(part_imgs[i]+cur_buf_index,(uint16_t*)&y,2);
        cur_buf_index+=2;
        memcpy(part_imgs[i]+cur_buf_index,(uint16_t*)&packet_size,2);
        cur_buf_index+=2;
        memcpy(part_imgs[i]+cur_buf_index,(uint16_t*)&i,2);
        cur_buf_index+=2;
        memcpy(part_imgs[i]+cur_buf_index,img_buffer+i*packet_size,packet_size);
        for (int j=0;j<packet_size;j++){
            printf("%c",*(part_imgs[i]+cur_buf_index+j));
        }
        printf("\n");
        for (int j=0;j<packet_size;j++){
            printf("%c",*(img_buffer+i*packet_size+j));
        }
        printf("\n\n%d,%d\n\n\n",i,packet_count);
        cur_buf_index+=packet_size;
        
        
    }
    for (int j=0;j<packet_size;j++){
            printf("%c",*(part_imgs[0]+HEADER_LEN_ANY+HEADER_LEN_IMG+j));
        }
    printf("\n");
    return part_imgs;
}

int  read_from_udp(partial_img* partial, char * buffer,int buffer_len){
    int cur_buf_index=0;
    if  (((uint8_t) buffer[0])!=TYPE_IMG){
        printf("NOT A UDP IMG PACKET!! Ignoring");
        return 0;
    }
    cur_buf_index+=1;
    partial->timestamp=*((time_t *)(buffer+cur_buf_index));
    cur_buf_index+=8;
    partial->x=*((uint16_t *)(buffer+cur_buf_index));
    cur_buf_index+=2;
    partial->y=*((uint16_t *)(buffer+cur_buf_index));
    cur_buf_index+=2;
    partial->len=*((uint16_t *)(buffer+cur_buf_index));
    cur_buf_index+=2;
    partial->seq_num=*((uint16_t *)(buffer+cur_buf_index));
    cur_buf_index+=2;
    partial->img_part=malloc(buffer_len-cur_buf_index);
    if (!partial->img_part){
        perror("ERROR : MALLOC ERROR : @ read_from_udp");
        exit(1);
    }
    memcpy(partial->img_part,buffer+cur_buf_index,buffer_len-cur_buf_index);
    return 1;

}
int init_partial_img(seq_img * img, partial_img * first_part){
    img->x=first_part->x;
    img->y=first_part->y;
    img->timestamp=first_part->timestamp;
    img->img=malloc(sizeof(char)*img->x*img->y*3);
    if (img->img){
        return 1;
    }
    perror("ERROR : MALLOC ERROR : @ init_partial_img");
    exit(1);
     
}
void destroy_img(seq_img * img){
    free(img->img);
}

