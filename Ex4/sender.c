#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include<time.h>

#define SERVER_PORT 8080
#define SIZE 2222
#define serverIP "127.0.0.1"
char container[SIZE];
socklen_t len;

void send_File(FILE *fp, int sockfd);
void send_5(char* c);

int main()
{
    send_5("cubic");
    send_5("reno");
}

// this function will send 5 times the file, as we asked to , in it we will change
void send_5(char* c){
    
    for (int i = 0; i < 5; i++)
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock == -1)
        {
            fprintf(stderr, "Unable to create this socket, error num: %s\n", strerror(errno));
            exit(EXIT_FAILURE); 
        }

        strcpy(container, c);
        len = sizeof(container);
        int socks = setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, container, len);
        printf("CC algorithm: %s\t", container);
        if(socks !=0 )
        {
            perror("setsockopt");
            exit(EXIT_FAILURE); 
        }

        //creating a new struct for IPV4 only
        struct sockaddr_in server_address;
        memset(&server_address, 0, sizeof(server_address)); //deleting first n chars of the string 
        server_address.sin_family = AF_INET; // stand for IPV4
        server_address.sin_port = htons(SERVER_PORT); //because the byte might be in different order in different computers we use this func
        int t = inet_pton(AF_INET, (const char*)serverIP, &server_address.sin_addr); //convert IPV4 to binary
        //if the convert didnt work, the return value will be 0 or -1, -1 says that there is a problem, 0 says that the transfer didnt work
        if(t <= 0) {
            printf("faild to convert IP");
            exit(1);
        }
    
       // connect with the server using TCP ( if the protocol is UDP we dont have to connect )
        int connection = connect(sock, (struct sockaddr *) &server_address, sizeof(server_address));
        if(connection == -1) {
            fprintf(stderr, "connection failed %s\n", strerror(errno));
            exit(EXIT_FAILURE); 
        } 
    
        FILE *fp = fopen("1gb.txt", "r");
        if (fp == NULL)
        {
            perror("Couldnt read file");
            exit(1);
        }

        send_File(fp, sock);

        sleep(1);
        close(sock);
    }
}

// this function will send in a TCP protocol ( as we asked to) using the "serial" number of the socket (sockfd)
void send_File(FILE *fp, int sockfd)
{
    int data_stream;
    int size = 0;
    while( ( data_stream = fread(container, 1, sizeof(container), fp) ) > 0 )
    {
        size += send(sockfd, container, data_stream, 0);
    }
    printf("File '1gb.txt' sent\n");
}