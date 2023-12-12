#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "upd_rasp.h"
#include "img-trnsfrt.h"
//#include <openssl/ssl.h>
	
#define PORT	 8080
#define MAXLINE 1024
#define STDIN_BUFFER_S 8196

// Driver code
int main() {
	int sockfd;
	char buffer[MAXLINE];
    char buffers[STDIN_BUFFER_S];
	char *hello = "Hello from server";
	struct sockaddr_in servaddr, cliaddr;
		
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
		
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = inet_addr("192.168.1.16");
	servaddr.sin_port = htons(PORT);
	
    // Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
        close(sockfd);
		exit(EXIT_FAILURE);
	}
   
    
	int n, len;
	struct pollfd pfd={sockfd,POLLIN,0};
    int timeout=10000;
    seq_img img;
	while (1){
        
        int res= poll(&pfd,1,timeout);
        if (res){
            int udb_buf_count,udp_buf_len;
            n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
            MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
            &len); 
                
            partial_img p_img;
            read_from_udp(&p_img,buffer,n);
            init_partial_img(&img,&p_img);
            for (int j=0;j<p_img.len;j++){
                printf("%c",p_img.img_part[j]);
            }  
                
            char response[10];
            memcpy(response,&p_img.timestamp,8);
            memcpy(response+8,&p_img.seq_num,2);
            
            int sent=sendto(sockfd, response, 10,  
                    MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
                    len); 
            printf("sent: %d\n",sent);
            free(p_img.img_part);
            }
           
        else {
            printf("received nothing :(\n");
        }
    }

	
	//puts("aaaaaaaa");
		
	return 0;
}