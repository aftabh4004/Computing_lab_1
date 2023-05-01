#include<stdio.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<string.h>
#define LEN 100

int main(){

    //Array for storing the file names of one or more file
    char file[LEN];
    printf("Enter the file name: ");
    scanf("%s", file);

    int pid;

    // string to store the command that has to be executed
    char cmd[LEN] = "";

    //Creating child process
    if((pid = fork()) == 0){ // Child process
        
        //Making of command
        strcat(cmd, "vi ");
        strcat(cmd, file);
        strcat(cmd, "; exec bash");
        

        //Opening vi editor inside the xterm
        execlp("xterm", "xterm", "-e", "vi", file,  NULL);
        printf("Something went wrong!");
        perror("Error");
    }
}
