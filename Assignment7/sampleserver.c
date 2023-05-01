#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
// #include<netinet/in.h>
// #include<sys/types.h>
#include<unistd.h>
int main(){

    struct sockaddr_in cli_addr, sever_addr;
    int socketfd, newsockfd;

    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Unable to create socket\n");
        exit(1);
    }

    sever_addr.sin_family = AF_INET;
    sever_addr.sin_addr.s_addr = INADDR_ANY;
    sever_addr.sin_port = 5000;

    if (bind(socketfd, (struct sockaddr *)&sever_addr, sizeof(sever_addr))){
        printf("Unable to bind\n");
        exit(1);
    }

    listen(socketfd, 5);

    int clilen;
    while (1){
        printf("Waiting for the connection\n");
        newsockfd = accept(socketfd, (struct sockaddr *)&cli_addr, &clilen);
        printf("Connected\n");
        if(newsockfd < 0){
            printf("Accepting error!\n");
            exit(1);
        }

        if(fork() == 0){
            char buff[100];

            close(socketfd);
            
            for(int i = 0; i < 100; i++){
                buff[i] = '\0';
            }
            strcpy(buff, "Message from server\n");
            send(newsockfd, buff, 100, 0);

            for(int i = 0; i < 100; i++){
                buff[i] = '\0';
            }

            recv(newsockfd, buff, 100, 0);
            printf("%s\n", buff);

            close(newsockfd);
            exit(0);

        }
        close(newsockfd);
    }
    


}