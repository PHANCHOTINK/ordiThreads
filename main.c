#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include<stdint.h>

typedef struct seq_img{
    uint16_t x;
    uint16_t y;
    time_t timestamp;
    char * img;

}seq_img;

typedef struct  // 4
{
    unsigned char messageType;
    u_int32_t messageIP;
    unsigned char messageObject;
    unsigned char X;
    unsigned char Y;    
    unsigned char ImageSize;
    char *dataValue;
}TransfertImage;

// Shared structure between UDP and TCP threads
typedef struct {
    pthread_mutex_t mutex;
    seq_img udpData;  // Data from UDP thread
    int udpDataReady; // Flag to signal data readiness
} SharedData;

SharedData sharedData = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .udpDataReady = 0,
};

void createImgFolder() {
    struct stat st = {0};

    if (stat("img", &st) == -1) {
        mkdir("img", 0700);
    }
}

void createCapteurFolder() {
    struct stat st = {0};

    if (stat("capteur", &st) == -1) {
        mkdir("capteur", 0700);
    }
}

void createPressoirFolder() {
    struct stat st = {0};

    if (stat("pressoir", &st) == -1) {
        mkdir("pressoir", 0700);
    }
}

void createDateImgFolder() {
    char dateFolder[20];

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(dateFolder, sizeof(dateFolder), "img/%Y-%m-%d", tm_info);

    struct stat st = {0};

    if (stat(dateFolder, &st) == -1) {
        mkdir(dateFolder, 0700);
    }
}

void createDateCapteurFolder() {
    char dateFolder[20];

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(dateFolder, sizeof(dateFolder), "capteur/%Y-%m-%d", tm_info);

    struct stat st = {0};

    if (stat(dateFolder, &st) == -1) {
        mkdir(dateFolder, 0700);
    }
}

void createDatePressoirFolder() {
    char dateFolder[20];

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(dateFolder, sizeof(dateFolder), "pressoir/%Y-%m-%d", tm_info);

    struct stat st = {0};

    if (stat(dateFolder, &st) == -1) {
        mkdir(dateFolder, 0700);
    }
}

// Function prototypes
void *tcpThread(void *arg);
void *udpThread(void *arg);
void *serialThread(void *arg);

int main() {
    pthread_t tcpThreadId, udpThreadId, serialThreadId;
    while(1){
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        int value;
        int appareil;
        printf("1 pour TCP \n2 pour UDP \n3 pour Serial \n4 pour arreter\n");
        scanf("%d",&value);

        switch(value){
            // Create threads for TCP, UDP, and Serial
            case 1:
                pthread_create(&tcpThreadId, NULL, tcpThread, NULL);
                break;
            case 2:
                pthread_create(&udpThreadId, NULL, udpThread, NULL);
                printf("retrieve img camera\n");
                // Create "img" folder if it doesn't exist
                createImgFolder();

                // Create a folder with the current date inside the "img" folder
                createDateImgFolder();

                // Create an image (replace this with your image creation code)
                printf("Creating image...\n");

                char filenameImg[50];
                strftime(filenameImg, sizeof(filenameImg), "img/%Y-%m-%d/image_%H%M%S.jpg", tm_info);

                // Replace the following line with your image saving code
                FILE *imageFile = fopen(filenameImg, "w");
                if (filenameImg != NULL) {
                    fclose(imageFile);
                    printf("UDP Thread finished. Image saved to '%s'.\n", filenameImg);
                } else {
                    printf("Error: Unable to save image.\n");
                }
                break;
            case 3:
                printf("1 pour capteur, 2 pour pressoir\n");
                scanf("%d",&appareil);
                pthread_create(&serialThreadId, NULL, serialThread, NULL);
                switch(appareil){
                    case 1:
                        //------------------------- capteur
                        printf("retrieve capteur");
                        // Create "capteur" folder if it doesn't exist
                        createCapteurFolder();

                        // Create a folder with the current date inside the "capteur" folder
                        createDateCapteurFolder();

                        // Create an image (replace this with your image creation code)
                        printf("Creating capteur log...\n");

                        char filenameCapteur[50];
                        strftime(filenameCapteur, sizeof(filenameCapteur), "capteur/%Y-%m-%d/logCapteur_%H%M%S.log", tm_info);

                        // Replace the following line with your image saving code
                        FILE *capteurFile = fopen(filenameCapteur, "w");
                        if (capteurFile != NULL) {
                            fclose(capteurFile);
                            printf("Serial Thread finished. Log saved to '%s'.\n", filenameCapteur);
                        } else {
                            printf("Error: Unable to save log.\n");
                        }
                        break;
                    case 2:
                        printf("retrieve pressoir\n");
                        // Create "capteur" folder if it doesn't exist
                        createPressoirFolder();

                        // Create a folder with the current date inside the "capteur" folder
                        createDatePressoirFolder();

                        // Create an image (replace this with your image creation code)
                        printf("Creating pressoir log...\n");

                        char filenamePressoir[50];
                        strftime(filenamePressoir, sizeof(filenamePressoir), "pressoir/%Y-%m-%d/logPressoir_%H%M%S.log", tm_info);

                        // Replace the following line with your image saving code
                        FILE *pressoirFile = fopen(filenamePressoir, "w");
                        if (pressoirFile != NULL) {
                            fclose(pressoirFile);
                            printf("Serial Thread finished. Log saved to '%s'.\n", filenamePressoir);
                        } else {
                            printf("Error: Unable to save log.\n");
                        }
                        break;
                    default:
                    printf("appareil qui n'existe pas\n");
                    break;
                }
                break;
            case 4:
                // Wait for threads to finish
                printf("je tue les threads\n");
                if(tcpThreadId!='\0'){
                    pthread_join(tcpThreadId, NULL);
                }
                if(udpThreadId!='\0'){
                    pthread_join(udpThreadId, NULL);
                }
                if(serialThreadId!='\0'){
                    pthread_join(serialThreadId, NULL);
                }
                return 0;
            default:
                printf("Signal qui n'existe pas\n");
                break;
        }
    }
}

void *tcpThread(void *arg) {
    // TCP thread code here
    printf("TCP Thread started.\n");

    while (1) {
        pthread_mutex_lock(&sharedData.mutex);

        // Check if UDP data is ready
        if (sharedData.udpDataReady) {
            // Copy UDP data to TCP structure
            TransfertImage tcpData;
            tcpData.messageType = 'U';
            tcpData.messageIP = 0;
            tcpData.messageObject = 'I';
            tcpData.X = sharedData.udpData.x;
            tcpData.Y = sharedData.udpData.y;
            tcpData.ImageSize = strlen(sharedData.udpData.img);
            tcpData.dataValue = strdup(sharedData.udpData.img);

            // Reset the flag
            sharedData.udpDataReady = 0;

            pthread_mutex_unlock(&sharedData.mutex);

            // Process or send the TCP data as needed
            printf("TCP Thread received UDP data. X: %u, Y: %u, ImageSize: %u\n", tcpData.X, tcpData.Y, tcpData.ImageSize);
            // Add your TCP logic here
        } else {
            pthread_mutex_unlock(&sharedData.mutex);
            // Sleep or perform other tasks as needed
            usleep(1000); // Sleep for 1 millisecond as an example
        }
    }

    printf("TCP Thread finished.\n");
    pthread_exit(NULL);
}

void *udpThread(void *arg) {
    // UDP thread code here
    printf("UDP Thread started.\n");

    // Simulate receiving data over UDP
    seq_img udpData;
    udpData.x = 123;
    udpData.y = 456;
    udpData.timestamp = time(NULL);
    udpData.img = "Sample Image Data";

    // Update the shared data structure
    pthread_mutex_lock(&sharedData.mutex);
    sharedData.udpData = udpData;
    sharedData.udpDataReady = 1;
    pthread_mutex_unlock(&sharedData.mutex);

    printf("UDP Thread finished.\n");
    pthread_exit(NULL);
}

void *serialThread(void *arg) {
    // Serial thread code here
    printf("Serial Thread started.\n");

    printf("Serial Thread finished.\n");
    pthread_exit(NULL);
}
