#ifndef __IMG_TRNSFRT_H__
#define __IMG_TRNSFRT_H__
#include <stdlib.h>
#include<time.h>
#include<stdint.h>
#include "udp_rasp.h"
/**
 * @brief Struct pour image partielle
 * pour transfert UDP
 * 
 */
typedef struct partial_img{
    uint16_t x;
    uint16_t y;
    uint16_t len;
    uint16_t seq_num;//position des bytes dans l'image
    time_t timestamp; // utilisé en tant qu'id avec le champ suivant
    uint8_t id; //
    char * img_part; //data
}partial_img;
/**
 * @brief Representation d'une image de facon sequentielle
 * i.e char *
 * + donnees en plus
 * 
 */
typedef struct seq_img{
    uint16_t x;
    uint16_t y;
    time_t timestamp;
    char * img;

}seq_img;
#define ADD_PARTIAL_OK 0
#define ADD_PARTIAL_END 1
#define ADD_PARTIAL_ERROR 2
/**
 * @brief Copie les donnees donnees par la partie 
 * de l'image et les insere dans l'image
 * 
 * @param img 
 * @param part_img 
 * @return int 
 */
int add_partial_img(seq_img * img, partial_img * part_img);
/**
 * @brief transforme une image sous forme 
 * char * en un ensemble de packet a envoyer sur le reseau
 * 
 * @param img_buffer 
 * @param buffer_len 
 * @param x 
 * @param y 
 * @param timestamp 
 * @param packet_size 
 * @param udp_buf_cout 
 * @param udp_buf_len 
 * @param id 
 * @return char** 
 */
char ** to_udp_buffer(char * img_buffer,int buffer_len,int x,int y, time_t timestamp, int packet_size,
                        int * udp_buf_cout, int * udp_buf_len,uint8_t id);
int  read_from_udp(partial_img* partial, char * buffer,int buffer_len);

/**
 * @brief Initialize a seq_img struct using the partial
 * img data
 * 
 * @param img 
 * @param first_part 
 * @return int 
 */
int init_partial_img(seq_img * img, partial_img * first_part);
/**
 * @brief destroy (and so malloc an image)
 * 
 * @param img 
 */
void destroy_img(seq_img * img);
#endif // __IMG-TRNSFRT_H__