#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>

int main(){
    int sockfd;
    struct sockaddr_in serv_addr;


    if((sockfd  = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Unable to create socket\n");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("10.145.226.105");
    serv_addr.sin_port = 5000;


    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Unable to connect to the server\n");
        perror("Error:");
        exit(0);
    }

    char buff[100];
    for(int i = 0; i < 10; i++){
        buff[i] = '\0';
    }
    recv(sockfd, buff, 100, 0);
    printf("Client received %s\n", buff);


    for(int i = 0; i < 10; i++){
        buff[i] = '\0';
    }
    strcpy(buff, "Message from the client\n");
    send(sockfd, buff, 100, 0);

}
