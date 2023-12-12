#include "udp_rasp.h"
#include <poll.h>
#include "img-trnsfrt.h"
#include <string.h>
#include<stdio.h>
int send_seq_img(int sockfd, const struct sockaddr* servaddr,socklen_t iplen,
        char * img_buffer,int buffer_len,int x,int y, time_t timestamp, int packet_size){

    int udb_buf_count,udp_buf_len;
    //printf("%p\n",img_buffer);
    char ** buffers = to_udp_buffer(img_buffer,strlen(img_buffer),x,y,timestamp,packet_size, 
                    &udb_buf_count,&udp_buf_len);
    //printf("%p\n",img_buffer);
  
    for (int i=0;i<udb_buf_count;i++){
        for (int j=0;j<packet_size;j++){
            //printf("%c",*(buffers[0]+17+j));
        }
        //printf("\n%s",buffers[i]);
        int received= sendto(sockfd, buffers[i], udp_buf_len,0, servaddr,iplen);    
        
        
        printf("send!! %d\n",received);
        
    }
    struct pollfd pfd={sockfd,POLLIN,0};
    int timeout=50;
    char inet_buffer[1000];
    while(poll(&pfd,1,timeout)){
        int n = recvfrom(sockfd, (char *) inet_buffer, 1000,  
                MSG_WAITALL, (struct sockaddr *) servaddr, 
                &iplen); 
        printf("got: %d & %d \n",*((u_int64_t*)(inet_buffer)),*((u_int16_t*)(inet_buffer+8)));
    }

}
