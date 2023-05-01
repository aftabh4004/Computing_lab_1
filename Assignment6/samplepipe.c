#include <stdio.h>
#include <unistd.h> /* Include this file to use pipes */
#define BUFSIZE 80
int main(){
    int fd[2], n=0, i;
    char line[BUFSIZE];
    pipe(fd); /* fd[0] is for reading, fd[1] is for writing */
    if (fork() == 0){
        close(1);
        dup(fd[1]) ; /* Redirect the stdout of this process to the pipe. */
        for (i=0; i < 10; i++) {
            printf("%d\n",n);
            n++;
            // sleep(2);
        }
    }
    else{
        close(0); 
        dup(fd[0]) ; /* Redirect the stdin of this process to the pipe */
        for (i=0; i < 10; i++) {
            /* The child will not read */
            scanf("%d",&n);
            printf("Parent reads: %d\n",n);
        } 
    }
}
