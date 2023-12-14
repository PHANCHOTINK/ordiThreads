#include "udp_rasp.h"
#include <poll.h>
#include "img-trnsfrt.h"
#include <string.h>
#include<stdio.h>
#include<sys/time.h>
extern uint8_t __curr_img_id=0;

/**
 * @brief Send an img stored as a char * on the network 
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
int send_seq_img(int sockfd, const struct sockaddr* servaddr,socklen_t iplen,
        char * img_buffer,int buffer_len,int x,int y, time_t timestamp, int packet_size){
    __curr_img_id=(__curr_img_id+1);
    int udb_buf_count,udp_buf_len;
    //printf("%p\n",img_buffer);
    char ** buffers = to_udp_buffer(img_buffer,strlen(img_buffer),x,y,timestamp,packet_size, 
                    &udb_buf_count,&udp_buf_len,__curr_img_id);
    //printf("%p\n",img_buffer);
  
    for (int i=0;i<udb_buf_count;i++){
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
    return 1;

}
int send_seq_img_s(udp_struct* udp_s,
        char * img_buffer,int buffer_len,int x,int y, time_t timestamp, int packet_size){
    return send_seq_img(udp_s->sockfd, (const struct sockaddr*) udp_s->servaddr,udp_s->iplen,
        img_buffer, buffer_len, x, y,  timestamp, packet_size);

}
int response_seq_img_s(udp_struct* udp_s,time_t timestamp, int seq_num){
    char response[10];
    memcpy(response,&timestamp,8);
    memcpy(response+8,&seq_num,2);
    send_udp(udp_s,response,10);
    
}
int send_udp(udp_struct* udp_s, char * buf, int data_len){
    int ret =sendto(udp_s->sockfd, buf, data_len,  
                MSG_CONFIRM, (const struct sockaddr *) udp_s->servaddr, 
                (udp_s->iplen));
    return ret;
}
int receive_udp(udp_struct* udp_s, char * buf, int data_len,int timeout){
    struct pollfd pfd={udp_s->sockfd,POLLIN,0};
    int res= poll(&pfd,1,timeout);
    if (res==0){
        return -2;
    }
    return recvfrom(udp_s->sockfd, buf, data_len,  
                0,udp_s->servaddr, 
                &udp_s->iplen);
}
int send_ping(udp_struct* udp_s, int try){
    char * msg=malloc(9);
    msg[0]=TYPE_PING;
    int res;
    //time_t * timestamps= malloc (try); 
    for (int i=0;i<try;i++){
        time_t cur_time=time(NULL);
        ((u_int64_t *)(msg+1))[0]=cur_time;
        res= send_udp(udp_s,msg,9);
    }
    free(msg);
    return res;

}

int resp_ping(udp_struct* udp_s,time_t timestamp){

    char msg[9];
    msg[0]=TYPE_PING_RESP;
    //time_t * timestamps= malloc (try); 
    time_t cur_tim=time(NULL);
    ((u_int64_t *)(msg+1))[0]=timestamp;
    printf("%d\n",(u_int8_t) msg[0]);
    return send_udp(udp_s,msg,9);

}
time_t get_timestamp(char * buffer){
    return *((time_t*) (buffer+1));
}

int send_cam_move(udp_struct* udp_s,cam_move move){
    char msg[5];
    msg[0]=TYPE_CAM_MOVE;

    ((u_int16_t *)(msg+1))[0]=move.left_move;
    ((u_int16_t *)(msg+1))[1]=move.up_move;

    return send_udp(udp_s,msg,5);
}
cam_move retrieve_cam_move(char * msg){
    cam_move move;
    move.left_move=((u_int16_t *)(msg+1))[0];
    move.up_move=((u_int16_t *)(msg+1))[1];
    return move;
}
int send_img_request(udp_struct* udp_s,time_t start, time_t end);
FILE * init_log(){
    time_t now= time(NULL);
    char buffer[200];
    sprintf(buffer,"%d.log",now);
    FILE * file =fopen(buffer,"w");
    return file;
}
int log_udp(FILE * file, char * buffer, int buf_len){
    time_t timestamp= time(NULL);
    char buffer[500];
    uint8_t type = buffer[0];
    char type_buffer[20];
    switch (type)
    {
    case TYPE_IMG:
        sprintf(type_buffer,"%s", "Type : img,");
        break;
    case TYPE_PING:
        sprintf(type_buffer,"%s", "Type : ping,");
        break;
    case TYPE_PARAM:
        sprintf(type_buffer,"%s", "Type : param,");
        break;
    case TYPE_IMG_BACKUP:
        sprintf(type_buffer,"%s", "Type : backup,");
        break;

    /*case TYPE_:
        sprintf(type_buffer,"%s", "Type : img,");
        break;*/
    case TYPE_PING_RESP:
        sprintf(type_buffer,"%s", "Type : ping resp,");
        break;
    default:
        sprintf(type_buffer,"%s%d,", "Unknown type : ",type);
        break;
    
    }
    return fprintf(file,"%s%d",type_buffer,timestamp );

}