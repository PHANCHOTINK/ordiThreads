#ifndef __UDP_RASP_H__
#define __UDP_RASP_H__
#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include <stdio.h>
#include"move.h"
//les types de paquets
#define TYPE_IMG 1
#define TYPE_PING 2
#define TYPE_PARAM 3
#define TYPE_IMG_BACKUP 4
#define TYPE_CAM_MOVE 5

#define TYPE_PING_RESP 128+TYPE_PING

#define HEADER_LEN_ANY (72/8)
#define HEADER_LEN_IMG (72/8)

/**
 * @brief regroupement de données
 * en vue de réduire le nombre de param
 * à donner en appel de f(x)
 * 
 */
typedef struct udp_struct{
    int sockfd;
    struct sockaddr  * servaddr;
    socklen_t iplen;
}udp_struct;
/**
 * @brief deprecie, utiliser la fonction terminant par un s
 * 
 * @param sockfd 
 * @param servaddr 
 * @param iplen 
 * @param img_buffer 
 * @param buffer_len 
 * @param x 
 * @param y 
 * @param timestamp 
 * @param packet_size 
 * @return int 
 */
int send_seq_img(int sockfd,const struct sockaddr  * servaddr,socklen_t iplen,
        char * img_buffer,int buffer_len,int x,int y, time_t timestamp, int packet_size);
/**
 * @brief envoie une image sur le réseau
 * 
 * @param udp_s 
 * @param img_buffer 
 * @param buffer_len 
 * @param x 
 * @param y 
 * @param timestamp 
 * @param packet_size 
 * @return int 
 */
int send_seq_img_s(udp_struct* udp_s,
        char * img_buffer,int buffer_len,int x,int y, time_t timestamp, int packet_size);
/**
 * @brief envoi un paquet udp sur le réseau
 * 
 * @param udp_s 
 * @param buf 
 * @param data_len 
 * @return int 
 */
int send_udp(udp_struct* udp_s, char * buf, int data_len);
int send_ping(udp_struct* udp_s, int try);
int receive_udp(udp_struct* udp_s, char * buf, int data_len,int timeout); 
int resp_ping(udp_struct* udp_s,time_t timestamp);
int send_cam_move(udp_struct* udp_s,cam_move move);
time_t get_timestamp(char * buffer);
cam_move retrieve_cam_move(char * msg);
//a faire
int send_img_request(udp_struct* udp_s,time_t start, time_t end);
FILE * init_log();
int log_udp(FILE * file, char * buffer, int buf_len);
#endif // __UDP_RASP_H__