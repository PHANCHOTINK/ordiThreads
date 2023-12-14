#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include "upd_rasp.h"
#include "img-trnsfrt.h"
//#include <openssl/ssl.h>
	
#define PORT	 8080
#define MAXLINE 1024
#define STDIN_BUFFER_S 8196

// Driver code
int main(int argc, char* argv[]) {
	int sockfd;
	char buffer[MAXLINE];
    char buffers[STDIN_BUFFER_S];
	char *hello = "Hello from server";
	struct sockaddr_in servaddr, cliaddr;
    char * addr;
		
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
		
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
    if (argc<=1){
        printf("Warning, no ip given, listening on 0.0.0.0");
        addr="0.0.0.0";
    }
    else{
        addr=malloc(4*4);
        addr=argv[1];
        printf("using ip addr : %s",addr);
    } 
	servaddr.sin_addr.s_addr = inet_addr(addr);
	servaddr.sin_port = htons(PORT);
	
    // Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
        close(sockfd);
		exit(EXIT_FAILURE);
	}
   
    
	int n, len=sizeof(cliaddr);
	struct pollfd pfd={sockfd,POLLIN,0};
    int timeout=1000; // number of msecond before stoping listeninh
    const time_t start_time=time(NULL);
    const int ping_delay; //delay between ping
    time_t next_ping= start_time+ping_delay;
    struct timeval first_img_packet;
    time_t img_id=0;
    seq_img img;
    uint8_t additionnal_id=0; //might have more than one image per second
    //ip info
    udp_struct remote;
    remote.sockfd=sockfd;
    remote.servaddr=  (struct sockaddr *) &cliaddr;
    remote.iplen=len;
    //for imgs
    int request_img=0,ask_param=0;

	while (1){
        if (time(NULL)>next_ping){
            next_ping+= ping_delay;
            send_ping(&remote,1);
        }
        if (request_img){

        }
        if (ask_param){

        }
        //faire en sorte qu'il envoie les vielles images!!!!!
        int read_bytes= receive_udp(&remote,buffer,MAXLINE,timeout);
        if (read_bytes>0){
            int udb_buf_count,udp_buf_len;
           
                
           
            char type=buffer[0];
            printf("type: %d \n",(unsigned char) type);
            switch (type)
            {
            case TYPE_IMG:
                partial_img p_img;
                int buff_len=read_from_udp(&p_img,buffer,read_bytes);
                if  (p_img.timestamp!=img_id
                        ||p_img.id!=additionnal_id ){
                    if (img_id!=0){
                        //send to thread
                    }

                    init_partial_img(&img,&p_img);
                    img_id=get_timestamp(buffer);
                    additionnal_id=p_img.id;
                    gettimeofday(&first_img_packet,NULL);
                    
                }
                response_seq_img_s(&remote,p_img.timestamp,p_img.timestamp);
                
                free(p_img.img_part); 
                break;
            case TYPE_PING:

                resp_ping(&remote,*((uint64_t*)(buffer+1)));

                break;
            
            default:
                break;
            }
            
            
            
            memset(buffer,0,read_bytes);
            }
           
        else {
            printf("received nothing :(\n");
        }
    }

	
	//puts("aaaaaaaa");
		
	return 0;
}