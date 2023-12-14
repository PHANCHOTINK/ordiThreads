#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
	
#define PORT	 8080
#define MAXLINE 1024
#define STDIN_BUFFER_S 8196
#define INET_BUFFER_S 8196

// Driver code
int main() {
	int sockfd;
	char buffer[MAXLINE];
    char stdin_buffer[STDIN_BUFFER_S];
    char inet_buffer[INET_BUFFER_S]="prout";
	char *hello = "Hello from client";
	struct sockaddr_in	 servaddr, cliaddr;
	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = inet_addr("192.168.6.104");
	servaddr.sin_port = htons(PORT);

    socklen_t iplen = sizeof(cliaddr);
    socklen_t len;
	ssize_t read_bytes=0;

	while ((read_bytes=read(STDIN_FILENO,stdin_buffer,STDIN_BUFFER_S))!=0){
        printf("bytes : %d\n",read_bytes);
        sendto(sockfd, stdin_buffer, read_bytes,0, (const struct sockaddr *) &servaddr,iplen);
        printf("%s\n",stdin_buffer);
        printf("msg sent!\nwrite to send\n");
        memset(stdin_buffer,0,STDIN_BUFFER_S);
        
        int n = recvfrom(sockfd, (char *)inet_buffer, MAXLINE,  
                MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len); 
        printf("%s\n",inet_buffer);
        printf("msg received!\nwrite to send\n");
        memset(stdin_buffer,0,STDIN_BUFFER_S);           
    }
	//sendto(sockfd, (const char *)hello, strlen(hello), MAXLINE, (const struct sockaddr *) &servaddr, sizeof(servaddr));
		
	
	
	close(sockfd);
	return 0;
}