#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3000
#define MAX_CONNECTIONS 5
#define BUFFER_SIZE sizeof(struct EtatAppareil)

typedef struct EtatAppareil //3
{
    unsigned char messageType;
    u_int32_t messageIP;
    unsigned char messageObject;
    unsigned char messageSize;
    unsigned char dataValue;
} EtatAppareil;

typedef struct TransfertImage // 4
{
    unsigned char messageType;
    u_int32_t messageIP;
    unsigned char messageObject;
    unsigned char X;
    unsigned char Y;    
    unsigned char ImageSize;
    char *dataValue;
}

typedef struct NotificationValeurCapteur //5 
{
    unsigned char messageType;
    u_int32_t messageIP;
    unsigned char messageObject;  
    unsigned char MessageSize;
    unsigned char dataValue;
}

typedef struct LogTransfert //6
{
    unsigned char messageType;
    u_int32_t messageSize;
    unsigned char message;
}

void processBinaryData(const char* binaryData, size_t dataSize);

void processBinaryData(const char* binaryData, size_t dataSize) {
    if (dataSize >= 3) {
        unsigned char messageType = binaryData[0];
        unsigned char messageSize = binaryData[1];
        unsigned char dataValue = binaryData[2];

        printf("DOnnee recues: msgType=%u, msgSize=%u, dataValue=%u\n", messageType, messageSize, dataValue);
    }
}

int main()
{
    int server_socket, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the specified address and port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CONNECTIONS) == -1)
    {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connections
    if ((new_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size)) == -1)
    {
        perror("Acceptance failed");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Close the server socket as we don't need it anymore
    close(server_socket);

    EtatAppareil messageToSend;
    messageToSend.messageType = 3; 
    messageToSend.messageIP = htonl(INADDR_LOOPBACK); 
    messageToSend.messageObject = 2;
    messageToSend.messageSize = 3;
    messageToSend.dataValue = 4;     

    char * messageAuClient= malloc(sizeof(EtatAppareil));
    if(!messageAuClient){
        printf("malloc error");
        exit(1);
    }
    int current_index=0;
    messageAuClient[current_index]=messageToSend.messageType;
    current_index+=1;
    memcpy(messageAuClient+current_index,&messageToSend.messageIP,4);
    current_index+=4;
    messageAuClient[current_index]=messageToSend.messageObject;
    current_index+=1;
    messageAuClient[current_index]=messageToSend.messageSize;
    current_index+=1;
    messageAuClient[current_index]=messageToSend.dataValue;

    // Send the message structure to the client
    send(new_socket,messageAuClient, sizeof(EtatAppareil), 0);

    // Receive and process messages
    while (1)
    {
        // Receive data from the client
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recv(new_socket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0)
        {
            perror("Connection closed");
            break;
        }

        processBinaryData(buffer, bytesRead);

        // Send back the received data as an acknowledgment
        send(new_socket, buffer, strlen(buffer), 0);
    }

    // Close the client socket
    close(new_socket);

    return 0;
}