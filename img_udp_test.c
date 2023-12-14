#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include "udp_rasp.h"
#define PORT	 8080
#define MAXLINE 1024
#define IN_BUFFER_S 8196
#define INET_BUFFER_S 8196

// Driver code
int main(int argc, char* argv[]) {
	int sockfd;
	
	char buffer_in[IN_BUFFER_S];
	struct sockaddr_in	 servaddr, cliaddr;
	char * addr;
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}


    if (argc<=1){
        printf("Error, server ip not given, exiting");
        exit(2);
    }
    else{
        addr=malloc(4*4);
        addr=argv[1];
        printf("using ip addr : %s",addr);
    } 

	
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = inet_addr(addr);
	servaddr.sin_port = htons(PORT);

    socklen_t iplen = sizeof(cliaddr);
    socklen_t len;
	ssize_t read_bytes=0;

    udp_struct remote;
    remote.sockfd=sockfd;
    remote.servaddr= (struct sockaddr *) &servaddr;
    memcpy(remote.servaddr,(struct sockaddr *) &servaddr,sizeof(struct sockaddr));
    remote.iplen=iplen;
    //send_seq_img(sockfd,(const struct sockaddr *) &servaddr,iplen,buf, strlen(txt),20,75,123456789,100);
	while(1){
        
        usleep(1000);
        
        int res= send_ping(&remote,1);
        printf("res:%d\n",res);
        res=receive_udp(&remote,buffer_in,IN_BUFFER_S,100);
        if (res<0){
            printf(":(\n");
            continue;
        }
        printf("ping : %d\n",((u_int64_t *)(buffer_in+1))[0]);
        u_int8_t type =(u_int8_t) buffer_in[0];
        switch (type)
        {
        case TYPE_PING_RESP:
            printf("ping : %d\n",((u_int64_t *)(buffer_in+1))[0]);
            break;
        
        default:
            printf("unknown format :( %d \n",buffer_in[0]);
            break;
        }
        memset(buffer_in,0,res);

        
    }
	//sendto(sockfd, (const char *)hello, strlen(hello), MAXLINE, (const struct sockaddr *) &servaddr, sizeof(servaddr));
		
	
	
	close(sockfd);
	return 0;
}