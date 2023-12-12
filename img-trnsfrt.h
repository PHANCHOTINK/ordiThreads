#ifndef __IMG_TRNSFRT_H__
#define __IMG_TRNSFRT_H__
#include <stdlib.h>
#include<time.h>
#include<stdint.h>
typedef struct partial_img{
    uint16_t x;
    uint16_t y;
    uint16_t len;
    uint16_t seq_num;
    time_t timestamp;
    char * img_part;
}partial_img;
typedef struct seq_img{
    uint16_t x;
    uint16_t y;
    time_t timestamp;
    char * img;

}seq_img;
#define ADD_PARTIAL_OK 0
#define ADD_PARTIAL_END 1
#define ADD_PARTIAL_ERROR 2

int add_partial_img(seq_img * img, partial_img * part_img);

char ** to_udp_buffer(char * img_buffer,int buffer_len,int x,int y, time_t timestamp, int packet_size,
                        int * udp_buf_cout, int * udp_buf_len);
int  read_from_udp(partial_img* partial, char * buffer,int buffer_len);

int init_partial_img(seq_img * img, partial_img * first_part);
void destroy_img(seq_img * img);
#endif // __IMG-TRNSFRT_H__