#ifndef __UDP_RASP_H__
#define __UDP_RASP_H__
#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
	
int send_seq_img(int sockfd,const struct sockaddr  * servaddr,socklen_t iplen,
        char * img_buffer,int buffer_len,int x,int y, time_t timestamp, int packet_size);
#endif // __UDP_RASP_H__