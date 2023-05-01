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
 * @param cmd1 : parent program to execute
 * @param param1 : param to be provided to the parent cmd program
 * @param cmd2 : child program to execute
 * @param param2 : param to be provided to the child cmd program
 * @param is_ampersand : 1 if program has to run in background.
 */
void execute_piped_command(char* cmd1, char* param1, char* cmd2, char* param2, int is_ampersand){
    int fd[2], pipe_pid1 = 0, pipe_pid2 = 0;
    pipe(fd);
    

    if(strcmp(cmd1, "cd")  == 0){
        //chdir() changes the pwd to the dir provided as argument
        if(chdir(param1) != 0)
            printf("Error: No such directory\n");

        if(param2[0] == '\0')
            param2 = NULL;
        close(fd[1]);
        if ((pipe_pid2 = fork()) ==  0){
            dup2(fd[0], 0);
            close(fd[1]);
            execlp(cmd2, cmd2, param2, NULL);
            //if execlp fail, means command is invalid
            printf("Second Command not found\n");
            exit(1);
        }
        

    }
    else if(strcmp(cmd1, "exit")  == 0){
        exit(0);

    }
    else{
        // if no param is empty string, We have to pass NULL in param to execlp
        if(param1[0] == '\0')
            param1 = NULL;
        if(param2[0] == '\0')
            param2 = NULL;
        
        // Forking for the first command
        if ((pipe_pid1 = fork()) ==  0){
            int savestdout = dup(1);
            dup2(fd[1],1);
            // close(0);
            execlp(cmd1, cmd1, param1, NULL);
            //if execlp fail, means command is invalid
            dup2(savestdout, 1);
            printf("%s Command not found\n", cmd1);
            exit(1);
        }
        
        // Closing writing end of pipe because it is not needed for the second command
        close(fd[1]);

        // Forking for second command
        if((pipe_pid2 = fork()) == 0){
            dup2(fd[0],0);
            
            execlp(cmd2, cmd2, param2, NULL);
            //if execlp fail, means command is invalid
            printf("%s Command not found\n", cmd2);
            exit(1);
        }
    }

    // Waiting for the two child process
    if (!is_ampersand){
        waitpid(pipe_pid1, NULL, 0);
        waitpid(pipe_pid2, NULL, 0);
    }
}


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

void trim_cmd(char* pipe_cmd[]){
    for (int i = 0; i < 2; i++){
        if (pipe_cmd[i] == NULL)
            continue;
        if(strlen(pipe_cmd[i]) == 0){
            pipe_cmd[i] = NULL;
            continue;
        }

        // pipe_cmd[i][strcspn(pipe_cmd[i], "\n\r")] = '\0';
        int j = 0;
        while (pipe_cmd[i][j] == ' ') j++;
        pipe_cmd[i] = &pipe_cmd[i][j];

        j = strlen(pipe_cmd[i]) - 1;
        while (pipe_cmd[i][j] == ' ' || pipe_cmd[i][j] == '\n' || pipe_cmd[i][j] == '\r') j--;
        pipe_cmd[i][j+1] = '\0';
    }
}

int main(){
    while(1){
        //prompt
        printf("shell> ");
        char cmdstr[CMD_LEN];   // to store the command provided by the user 
        int is_ampersand[2] = {0};      // 1 if the program has to be run in background,
        int is_piped = 0;         // 1 if the command has a pipe operator,

        // reading command from stdin

        // If fgets gets the EOF
        if (fgets(cmdstr, CMD_LEN, stdin) == NULL){
            break;
        }
        // eliminating new line character
        cmdstr[strcspn(cmdstr, "\n\r")] = '\0';
        
        

        char* pipe_cmd[2], *dupcmd;
        dupcmd = strdup(cmdstr);
        pipe_cmd[0] = strsep(&dupcmd, "|");
        pipe_cmd[1] = dupcmd;

        trim_cmd(pipe_cmd);

        if(pipe_cmd[0] != NULL && pipe_cmd[1] != NULL){
            is_piped = 1;
        }
        else if(pipe_cmd[0] == NULL && pipe_cmd[1] == NULL){
            continue;
        }
        else if (pipe_cmd[0] == NULL){
            printf("Syntax error\n");
            continue;
        }

        //Search for '&' and replace by null character and set the is_ampersand variable
        
        for (int j = 0; j < 2; j++){
            if(pipe_cmd[j] == NULL)
                continue;
            int i = 0;
            while(pipe_cmd[j][i] != '\0'){
                if(pipe_cmd[j][i] == '&'){
                    is_ampersand[j] = 1;
                    pipe_cmd[j][i] = '\0';
                }
                i++;
            }
    
        }
        if (is_ampersand[0] == 1 && is_piped == 1){
            printf("Syntax error\n");
            continue;
        }
       
        // printf("%s\n%s\n", pipe_cmd[0], pipe_cmd[1]);


        // Seperating command and its parameters

        char* token, cmd1[CMD_LEN], param1[CMD_LEN], cmd2[CMD_LEN], param2[CMD_LEN];

        // Initialize the cmd and param char array
        for(int i = 0; i < CMD_LEN; i++) cmd1[i] =  param1[i] = cmd2[i] =  param2[i] = '\0';

        //Extracing program name and parameter from the command string
        token = strtok(pipe_cmd[0], " ");
        if(token == NULL){
            continue;   
        }

        strcpy(cmd1, token);


        
        token = strtok(NULL, "\0");
        if (token != NULL)
            strcpy(param1, token);
        

        // For second command, if pipe is used

        if (is_piped == 1){
            token = strtok(pipe_cmd[1], " ");
            if(token == NULL){
                continue;   
            }

            strcpy(cmd2, token);


            token = strtok(NULL, "\0");
            if (token != NULL)
                strcpy(param2, token);
            
        }


        //execute the command
        if(is_piped)
            execute_piped_command(cmd1, param1, cmd2, param2, is_ampersand[1]);
        else
            execute_command(cmd1, param1, is_ampersand[0]);
    }
}