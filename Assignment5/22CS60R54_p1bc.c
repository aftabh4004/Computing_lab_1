#include<stdio.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<signal.h>
#define LEN 100
#define CMD_LEN 25
#define MAX_TOKENS 50


//Struct to store the child pid and coresponding opened file name 
struct fileprocess{
    int pid;
    char* filemane;
    struct fileprocess* next;
};
typedef struct fileprocess fileprocess;


// Tokenises the read command with the delimter one blank space 
int commnad_tokenizer(char* cmdstr, char* cmdtokens[]){
    char* token;
    int k = 0;
    token = strtok(cmdstr, " ");

    while(token != NULL){
        char *tempstr = (char*)malloc(sizeof(char)*CMD_LEN);
        strcpy(tempstr, token);
        cmdtokens[k++] = tempstr;
        token = strtok(NULL, " ");
    }
    return k;
} 



// Opens file and maintain the opnfile list
fileprocess* open_file(fileprocess* opnfile, char* file){
    
    int pid;

    //Creating child process and open file by running vi in xterm
    if((pid = fork()) == 0){
        
        execlp("xterm", "xterm", "-e", "vi", file,  NULL);
        printf("Something went wrong\n");
        perror("Error");
    }


    // adding a node at the front of opnfiles linked list
    fileprocess* node = (fileprocess*)malloc(sizeof(fileprocess));
    node->filemane = file;
    node->pid = pid;
    node->next = NULL;
    if (!node){
        printf("cant make node\n");
    }
    if(opnfile == NULL){
        opnfile = node;
    }
    else{
        node->next = opnfile;
        opnfile = node;
    }
    return opnfile;
}


//Close the file with the name "file"
fileprocess* close_file(fileprocess* opnfile, char* file){
    fileprocess* cur = opnfile, *prev = NULL;

    // if the opnfile is empty, it means no file is opened
    if(!cur){
        printf("No file opend\n");
        return opnfile;
    }


    //Search for the file that has to be deleted
    // Kill the corresponding process using pid and delete the node from the opnfile list
    while(cur){
        if(strcmp(file, cur->filemane) == 0){
            if (prev == NULL){
                opnfile = cur->next;
                int res = kill(cur->pid, SIGKILL);
                if(res == -1){
                    perror("kill");
                }
                free(cur);
            }else{
                prev->next = cur->next;
                int res = kill(cur->pid, SIGKILL);
                if(res == -1){
                    perror("kill");
                }
                free(cur);
            }
            return opnfile;
        }
        prev = cur;
        cur = cur->next;
    }
    
    //Print message if the file is not found
    printf("Error: %s, No Such file\n", file);
    return opnfile;
}

int main(){
    
    //List of open files
    fileprocess* opnfile = NULL;
    while(1){


        char cmdstr[LEN];       // To store the command given by the user
        char* cmdtokens[MAX_TOKENS] = {NULL};       //array of tokens(string)
        printf("file > ");      //prompt

        //reading command
        fgets(cmdstr, LEN, stdin);

        //Replacing the first \n or \r to \0
        cmdstr[strcspn(cmdstr, "\r\n")] = '\0';
        int ntokens = commnad_tokenizer(cmdstr, cmdtokens);
        
        if(ntokens == 0){
            continue;
        }
        char* cmd = cmdtokens[0];
        
        if (strcmp(cmd, "open") == 0){
            if(ntokens <= 1){
                printf("Please list the file names\n");
                continue;
            }
            //Open every fiie one by one
            for(int i = 1; i < ntokens; i++){
                opnfile = open_file(opnfile, cmdtokens[i]);
            }
        }
        else if(strcmp(cmd, "close") == 0){
            
            if(ntokens <= 1){
                printf("Please list the file names\n");
                continue;
            }

            //close each file one by one
            for(int i = 1; i < ntokens; i++){
                opnfile = close_file(opnfile, cmdtokens[i]);
            }   
        }
        else if (strcmp(cmd, "quit") == 0){

            //Break the loop on exit
            break;
        }
        else{
            printf("Invalid command\n");
        }  
    }
}
