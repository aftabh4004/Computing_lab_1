#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<string.h>
#define BUF 100


/*
 * @des : Function to validate the input string, it parse the input string and check for valid four double values
 * @param: points : string contain the raw input received from the user
 * */
int validate_input(char* points){
    //count number of tokens
    int nt = 0;

    char* token = strtok(points, " ");
    // for each non empty tokens
    while(token != NULL && token[0] != '\0' && token[0] != '\n' && token[0] != '\r'){
        nt++;
        int i = 0, j = strlen(token) - 1, nd = 0, np = 0;

        // skip initial white spaces
        while(token[i] == ' ' & token[i] != '\0') i++;
        
        //skip last white spaces
        while(token[j] == ' '){
            token[j] = '\0';
            j--;
        } 
            
        token = &token[i];

        // if the token have negative double value
        if (token[0] == '-') i++;

        // check for any invalid symbols
        while(token[i] != '\0' && token[i] != '\n' && token[i] != '\r'){
            if (token[i] >= '0' && token[i] <= '9')
                nd++;
            else if (token[i] == '.')
                np++;
            else
                return 0;
            i++;
        }

        // one token can have only one decimal point
        if (np >= 2)
            return 0;

        // it must have atleast one digit
        if (nd <= 0)
            return 0;
        token = strtok(NULL, " ");
            
    }

    // there must be four values
    if(nt != 4)
        return 0;
    return 1;
}

int main(){

    // Creating pipe
    int fd[2], pid;
    pipe(fd);

    if((pid = fork()) == 0){
        
        // duplicate stdin of the child with the pipe reading end
        close(0);
        dup(fd[0]);
        
        // Closing fd which are not required
        close(1);
        close(fd[1]);

        execlp("gs", "gs", NULL);

        printf("Something went wrong\n");
        perror("Error");
        exit(1);
    }
    else{
        // pipe reading end is not needed
        close(fd[0]);


        while(1){
            double x1, x2, y1, y2;
            char choice;
            char cmd[BUF];
            printf("Give the coordinates of endpoints of line as x1 y1 x2 y2: ");
            char temp[BUF];
            fgets(temp, BUF, stdin);
            
            char* dupstr = strdup(temp);
            if(validate_input(dupstr) == 0){
                printf("Invalid input!\n");
                continue;
            }
            sscanf(temp, "%lf %lf %lf %lf", &x1, &y1, &x2, &y2);
                

            // creating command string
            sprintf(cmd, "%.2lf %.2lf moveto %.2lf %.2lf lineto closepath stroke\n", x1, y1, x2, y2);
            write(fd[1], cmd, BUF);


            printf("Wanna draw more lines?[y/n]: ");
            choice = fgetc(stdin);
            // To read extra newline
            fgetc(stdin);

            if(choice == 'y' || choice == 'Y')
                continue;
            else{
                printf("Closing program...\n");
                break;
            }
        }
    }
    // closing the parent writing end of pipe.
    close(fd[1]);
}
