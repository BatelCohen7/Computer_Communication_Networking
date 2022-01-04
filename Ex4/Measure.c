#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include<time.h>

#define SERVER_PORT 8080 //The same port in the client
#define SIZE 2222

void get_file(int sockfd);
double recive_Five(int listeningSocket);

int main()
{

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        printf("Error in creating Server socket\n");
        exit(1);
    }
    else
        printf("Server socket has been created successfully\n");

    
    int reUse = 1;
    
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &reUse, sizeof(int)) < 0)
    {
        printf("setsochet has been failed\n");
        exit(1);
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT); // host to network short

    // Bind the socket with the IP address and port
    if (bind(server_sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        printf("Binding has been failed\n");
        exit(1);
    }
    printf("Bind success\n");


    // After the bind success, we can listen to income information from our sender socket

    // 500 is the max number of connections requests
    if (listen(server_sock, 500) == -1) 
                                            
    {
        printf("Listen has been failed\n");
        close(server_sock);
        exit(1);
    }
    
    printf("Waiting for TCP connections...\n");


    //now we starting to recive the wanted file 5 time and start to measure
    double totalSum = recive_Five(server_sock);
    double CubicAVG = totalSum/5;


    //now we starting to recive the wanted file 5 time and start to measure
    totalSum = recive_Five(server_sock);
    double RenoAVG = totalSum/5;


    //Printing the avrage time of cubic and reno
    printf("\nCubic average time : %f\n",CubicAVG);
    printf("Reno average time : %f\n",RenoAVG);


    // after we measure we can close our server 
    close(server_sock);
    return 0;
}

//getting new file
void get_file(int sockfd)
{
    int n = -1;
    char container[SIZE];

    while(n = recv(sockfd, container, SIZE, 0) != 0)
    {
        bzero(container, SIZE);
    }
    return;
}

double recive_Five(int socket)
{
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);

    double totalSum = 0;
    for (int i = 0; i < 5; i++)
    {
        memset(&clientAddress, 0, sizeof(clientAddress));
        clientAddressLen = sizeof(clientAddress); // we need to update the new length in every iteration
        //the accept function get our connection and return the specific socket of this connection
        int sock2 = accept(socket, (struct sockaddr *)&clientAddress, &clientAddressLen);
        if(sock2 == -1) {
            printf("listen has been failed");
            close(socket);
            exit(0);
        }
         struct timespec start, end;
         clock_gettime(CLOCK_REALTIME, &start);
         get_file(sock2);
         clock_gettime(CLOCK_REALTIME, &end);
         double preserT = (end.tv_sec - start.tv_sec)+ (end.tv_nsec - start.tv_nsec)/ 1000000000.0;
        printf("The time it take is: %lf seconds \n", preserT);
        totalSum += preserT;
        sleep(1);
        // clock_t time1= clock();
        // get_file(sock2);
        // clock_t time2=clock();
        // double presentT=(double)(time2-time1)/CLOCKS_PER_SEC;
        // // double presenT = ((double)(time2-time1))/CLOCKS_PER_SEC;
        // printf("The time it take is : %.3F seconds\n", presentT);
        // totalSum += presentT;
        // sleep(1);
    }
    return totalSum;
}