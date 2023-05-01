#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<string.h>
#include<stdlib.h>
#include<sys/wait.h>
#define CMD_LEN 100


/**
 * @brief Function to execute command 
 * 
 * @param cmd : program to execute
 * @param param : param to be provided to the cmd program
 * @param is_ampersand : 1 if program has to run in background.
 */
void execute_command(char* cmd, char* param, int is_ampersand){
    int pid = -1;


    if(strcmp(cmd, "cd")  == 0){
        //chdir() changes the pwd to the dir provided as argument
        if(chdir(param) != 0)
            printf("Error: No such directory\n");
        return;
    }
    else if(strcmp(cmd, "mkdir") == 0){
        if((pid = fork()) == 0){
            execlp(cmd, cmd, param, NULL);

            //if the execlp fails
            perror("Error");
            exit(1);
        }
    }
    else if(strcmp(cmd, "ls") == 0){
        
        // if ls is not provided with any paramenter, "." denote the current directory
        if(param[0] == '\0')
            strcpy(param, ".");

        if((pid = fork()) == 0){
            execlp(cmd, cmd, param,  NULL);
            perror("Error");
            exit(1);
        }
    }
    else if(strcmp(cmd, "pwd") == 0){
        if((pid = fork()) == 0){
            execlp(cmd, cmd,  NULL);
            perror("Error");
            exit(1);
        }
    }
    
    else if(strcmp(cmd, "exit") == 0){
        //exit the program on getting "exit" as command
        exit(0);
    }
    else{
        // if no param is empty string, We have to pass NULL in param to execlp
        if(param[0] == '\0')
            param = NULL;
        if((pid = fork()) == 0){
            execlp(cmd, cmd, param, NULL);

            //if execlp fail, means command is invalid
            printf("Command not found\n");
            exit(1);
        }
    }

    //waits for the child process if program is not to run in background
    if(!is_ampersand)
        waitpid(pid, NULL, 0);
}

int main(){
    while(1){
        //prompt
        printf("shell> ");
        char cmdstr[CMD_LEN];   // to store the command provided by the user 
        int is_ampersand = 0;      // 1 if the program has to be run in background,

        // reading command from stdin
        fgets(cmdstr, CMD_LEN, stdin);
        // eliminating new line character
        cmdstr[strcspn(cmdstr, "\n\r")] = '\0';
        

        //Search for '&' and replace by null character and set the is_ampersand variable
        int i = 0;
        while(cmdstr[i] != '\0'){
            if(cmdstr[i] == '&'){
                is_ampersand = 1;
                cmdstr[i] = '\0';
            }
            i++;
        }


        char* token, cmd[CMD_LEN], param[CMD_LEN];

        // Initialize the cmd and param char array
        for(int i = 0; i < CMD_LEN; i++) cmd[i] =  param[i] = '\0';

        //Extracing program name and parameter from the command string
        token = strtok(cmdstr, " ");
        if(token == NULL){
            continue;   
        }

        strcpy(cmd, token);


        token = strtok(NULL, "\0");
        if (token != NULL)
            strcpy(param, token);
        
        //execute the command
        execute_command(cmd, param, is_ampersand);   
    }
}