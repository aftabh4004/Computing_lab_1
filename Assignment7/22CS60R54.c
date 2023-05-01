#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>


#define LISTEN_LIMIT 10
#define BUFSIZE 100
#define MAXNUMMAT 10000


// ===============================================================================//
// ===================   Multi-threaded matrix multiplication ====================//
// ===============================================================================//


// struct to store matrix
typedef struct MAT {
    long long int **mat;
    int r, c;
} MAT;


// Two Global array of matrices to calculate the matix multiplication
MAT* A[MAXNUMMAT];
MAT* B[MAXNUMMAT];

// Varible used to do thread sync
int blen;
int aready;
int done;


//pthread entity
pthread_mutex_t mutexblen;
pthread_mutex_t mutexdone;
pthread_barrier_t barrierBready;
pthread_barrier_t barrierAready;
pthread_barrier_t barrierServer;




// Function to create matrix of the given row and col by dynamic memory allocation.
MAT* create_mat(int r, int c){
    MAT* M = (MAT*) malloc(sizeof(MAT));
    M->r = r;
    M->c = c;
    M->mat = (long long int**) malloc(sizeof(long long int*) * r);
    for(int i = 0; i < r; i++){
        M->mat[i] = (long long int*) malloc(sizeof(long long int) * c);
    }
    return M;
}

void free_mat(MAT* m){
    if(m == NULL)
        return;
    for(int i = 0; i < m->r; i++){
        free(m->mat[i]);
    }
}


// Create a copy of provided matrix.
void duplicate_mat(MAT *a, MAT *c){   
    for (int i = 0; i < a->r; i++){
        for(int j = 0; j < a->c; j++){
            c->mat[i][j] = a->mat[i][j];
        }
    }
}


// Function to calculate multiplication of two matrices a and b and store the result into c.
void matmul(MAT *a, MAT *b, MAT* c){
    for (int i = 0; i < a->r; i++){
        for(int j = 0; j < b->c; j++){
            long long int sum = 0;
            for(int k = 0; k < a->c; k++){
                sum += (a->mat[i][k] * b->mat[k][j]);
            }
            c->mat[i][j] = sum;
        }
    }
}




// Routine for thread inplementation. 
// Each thread will multiply two matrix from A and store into B, by taking care of the indices.
void* matmulhandler(void* arg){
    int i = *(int*) arg;
    pthread_mutex_lock(&mutexdone);
    while(!done){
        pthread_mutex_unlock(&mutexdone);
        pthread_barrier_wait(&barrierAready);
        if(A[i] != NULL){
            MAT *a, *b, *c;

            // Getting the pointer to the matrices that has to be multiplied   
            a = A[i];
            b = A[i+1];

            if (b == NULL){
                c = create_mat(a->r, a->c);
                duplicate_mat(a, c);
            }
            else{
                c = create_mat(a->r, b->c);
                matmul(a, b, c);
            }
    
            // Storing the result in B
            B[i/2] = c;

            // Incrementing blen in thread safe manner
            pthread_mutex_lock(&mutexblen);
            blen++;
            pthread_mutex_unlock(&mutexblen);
        }
        pthread_barrier_wait(&barrierBready);
    }

    pthread_mutex_unlock(&mutexdone);
}



// Function to multiply a given number of matrices by creating multiple thread,
// Each thread will multiply two matrix from A and store into B, by taking care of the indices.

void parallel_matmul(int mat_n){
    int alen;
    alen = mat_n;
    int num_t;
    if(alen % 2 == 0)
        num_t = alen/2;
    else
        num_t = alen/2 + 1;
    pthread_t th[num_t];


    //pthread init
    pthread_mutex_init(&mutexblen, NULL);
    pthread_mutex_init(&mutexdone, NULL);
    pthread_barrier_init(&barrierBready, NULL, num_t + 1);
    pthread_barrier_init(&barrierAready, NULL, num_t + 1);



    // Creating threads

    for(int i = 0; i < num_t; i++){
        int *indx = (int*) malloc(sizeof(int));
        *indx = 2 * i;
        if (pthread_create(&th[i], NULL, matmulhandler, indx)){
            printf("Unable to create thread\n");
            exit(1);
        }

    }

    // Implementing barrier
    pthread_barrier_wait(&barrierAready);
    while(alen > 1){
        pthread_barrier_wait(&barrierBready);

        for(int i = 0; i < alen; i++){
            free_mat(A[i]);
            if(i < blen){
                A[i] = B[i];
                B[i] = NULL;
            }
            else
                A[i] = NULL;
        }
        alen = blen;
        blen = 0;
        pthread_barrier_wait(&barrierAready);
    }
    
    pthread_mutex_lock(&mutexdone);
    done = 1;
    pthread_mutex_unlock(&mutexdone);
    pthread_barrier_wait(&barrierBready);
    
    for(int i = 0; i < num_t; i++){
        if (pthread_join(th[i], NULL)){
            printf("Unable to join thread\n");
            exit(1);
        }

    }


    pthread_mutex_destroy(&mutexblen);
    pthread_mutex_destroy(&mutexdone);
    pthread_barrier_destroy(&barrierBready);
    pthread_barrier_destroy(&barrierAready);

}

// ===============================================================================//
// ===================   Master slave model  =====================================//
// ===============================================================================//



// Struct to pass required data to the client servicing thread
typedef struct task {
    int socketfd;
    int start, end;
    int tid;
} task;


// =========================== Server (Master) =============================================//



// Server Protocol: defines the rule to sending and receving data from the client

void* server_protocol(void * arg){
    task* t = (task*)arg;
    int socketfd = t->socketfd;
    int start = t->start, end = t->end, status;
    int tid = t->tid;
    long long int buf[BUFSIZE];
    int mat_n = end - start + 1;


   
    buf[0] = mat_n;
    send(socketfd, buf, BUFSIZE*sizeof(long long int), 0);

    for(int m = start; m <= end; m++){
        int r = A[m]->r, c = A[m]->c;
        buf[0] = r;
        buf[1] = c;
        send(socketfd, buf, BUFSIZE*sizeof(long long int), 0);

        int i = 0, j = 0;
        while(i < r){
            for(int k = 0; k < BUFSIZE && i < r; k++){
                buf[k] = A[m]->mat[i][j];
                j++;
                if(j >= c){
                    i++;
                    j = 0;
                }
            }
            
            send(socketfd, buf, BUFSIZE*sizeof(long long int), 0);
        }
        
    }

    int r, c;
    
    recv(socketfd, buf, BUFSIZE*sizeof(long long int), 0);
    r = buf[0]; c = buf[1];
    B[tid] = create_mat(r, c);

    int i = 0, j = 0;
    while(i < r){
        
        recv(socketfd, buf, BUFSIZE*sizeof(long long int), 0);
        for(int k = 0; k < BUFSIZE && i < r; k++){
            B[tid]->mat[i][j] = buf[k];
            j++;
            if(j >= c){
                i++;
                j = 0;
            }
        }
    }
    
    pthread_mutex_lock(&mutexblen);
    blen++;
    pthread_mutex_unlock(&mutexblen);



    pthread_barrier_wait(&barrierServer);
    pthread_exit(arg);
}



// Function to initialize the server.

int server_init(char* ip, int port, struct sockaddr_in* s_addr){
    int socketfd;
    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Unable to open socket.\n");
        exit(1);
    }

    s_addr->sin_family = AF_INET;
    s_addr->sin_addr.s_addr = inet_addr(ip);
    s_addr->sin_port = port;

    if(bind(socketfd, (struct sockaddr*)s_addr, sizeof(struct sockaddr)) < 0){
        printf("Unable to bind.\n");
        perror("Error:");
        exit(1);
    }

    listen(socketfd, LISTEN_LIMIT);
    printf("Server stated on ip %s:%d\n", ip, port);
    return socketfd;
}


// Function that will connect with the client and create the client serving thread.
void serves_clients(int socketfd, int nc, int mat_n, int socketfds[], struct sockaddr_in c_addr[]){
    int clilen;
    int len = mat_n/nc;
    pthread_t th[nc];

    pthread_barrier_init(&barrierServer, NULL, nc + 1);

    for(int i = 0; i < nc; i++){
        printf("Waiting for Client %d\n", i);
        socketfds[i] = accept(socketfd, (struct sockaddr*)&c_addr[i], &clilen);
        printf("Connected with client %d:\n", i);

        task * t = (task*) malloc(sizeof(task));
        t->socketfd = socketfds[i];
        t->start = i * len;
        if(t->start + 2 * len  <= mat_n)
            t->end = t->start + len - 1;
        else
            t->end = mat_n - 1;
        t->tid = i;
        if (pthread_create(&th[i], NULL, server_protocol, (void *)t)){
            printf("Unable to create thread for serving client %d\n", i + 1);
            exit(1);
        }
        
    }

    pthread_barrier_wait(&barrierServer);

    if(blen != nc)
        printf("Imcompleate B\n");
    for(int i = 0; i < mat_n; i++){
        free_mat(A[i]);
        A[i] = NULL;
        if(i < blen){    
            A[i] = B[i];
            B[i] = NULL;
        }
    }
    
    for(int i = 0; i < nc; i++){
        if(pthread_join(th[i], NULL)){
            printf("Unable to join \n");
            exit(1);
        }
    }

    parallel_matmul(blen);



    // Printing the final ans matix after the multiplication
    printf("Final Mat\n");
    MAT* ans = A[0];
    printf("%d %d\n", ans->r, ans->c);
    for(int i = 0; i < ans->r; i++){
        for(int j = 0; j < ans->c; j++){
            printf("%5lld ", ans->mat[i][j]);
        }
        printf("\n");
    }

    pthread_barrier_destroy(&barrierServer);


}


// Function used by server to read the matices from the file. 
int read_mat(char * path){
    int mat_n;
    FILE* fp = fopen(path, "r");
    if(!fp){
        printf("Unable to open input file\n");
        exit(1);
    }

    fscanf(fp, "%d", &mat_n);

    for(int i = 0; i < mat_n; i++){
        int r, c;
        fscanf(fp, "%d %d",&r, &c);
        if (i != 0 && A[i - 1]->c != r){
            printf("Invalid matrix sequence, Multiplication not possible\n");
            exit(1);
        }
        MAT* tm = create_mat(r, c);
        for(int j = 0; j < r; j++){
            for(int k = 0; k < c; k++){
                fscanf(fp, "%lld", &tm->mat[j][k]);
            }
        }
        A[i] = tm;
    }
    return mat_n;
}



// =========================== Client (Slave) =============================================//


// Protocol run on the client side to set the rule for sending and receving data.
void client_protocol(int socketfd){
    long long int buf[BUFSIZE];
    int status, mat_n;

    recv(socketfd, buf, BUFSIZE*sizeof(long long int), 0);
    mat_n = buf[0];

    for(int m = 0; m < mat_n; m++){
        int r, c;
    
        recv(socketfd, buf, BUFSIZE*sizeof(long long int), 0);
        r = buf[0]; c = buf[1];
        A[m] = create_mat(r, c);

        int i = 0, j = 0;
        while(i < r){
            
            recv(socketfd, buf, BUFSIZE*sizeof(long long int), 0);
            for(int k = 0; k < BUFSIZE && i < r; k++){
                A[m]->mat[i][j] = buf[k];
                j++;
                if(j >= c){
                    i++;
                    j = 0;
                }
            }
        }
    }
    parallel_matmul(mat_n);


    // Sending ans matrix to server

    int r = A[0]->r, c = A[0]->c;
    buf[0] = r;
    buf[1] = c;
    send(socketfd, buf, BUFSIZE*sizeof(long long int), 0);

    int i = 0, j = 0;
    while(i < r){
        for(int k = 0; k < BUFSIZE && i < r; k++){
            buf[k] = A[0]->mat[i][j];
            j++;
            if(j >= c){
                i++;
                j = 0;
            }
        }
        
        send(socketfd, buf, BUFSIZE*sizeof(long long int), 0);
    }

    printf("Successfully compleated. Exiting...\n");
}


// function to initialized the client and connect to the server
int client_connection_init(char* ip, int port, struct sockaddr_in* s_addr){
    int socketfd;
    
    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("[ Client ] Unable to create socket\n");
        exit(1);
    }

    s_addr->sin_addr.s_addr = inet_addr(ip);
    s_addr->sin_port = port;
    s_addr->sin_family = AF_INET;

    printf("Connecting to server... on %s:%d\n", ip, port);
    if(connect(socketfd, (struct sockaddr*)s_addr, sizeof(struct sockaddr)) < 0){
        printf("[ Client ] Unable to connect to the server\n");
        exit(1);
    }

    printf("Connect to the server!\n");
    client_protocol(socketfd);
}


// ====================================================================//
// ====================================================================//
// ======================= Main ======================================//

int main(int argc, char* argv[]){
    // Default values
    int is_server = 0, is_client = 0, is_nc = 0, is_path = 0;
    char ip_def[] = "127.0.0.1"; 
    char* ip = ip_def;
    char path_def[] = "input.txt";
    char* path = path_def; 
    int port = 6000;
    int nc = 5;
    int socketfds[nc];
    struct sockaddr_in c_addr[nc], s_addr;

    for(int i = 1; i < argc; i++){
        char* dup = strdup(argv[i]);
        char* p = strsep(&dup, "=");
        char* v = dup;

        if(!strcmp(p, "-s"))
            is_server = 1;
        else if(!strcmp(p, "-c"))
            is_client = 1;

        else if(!strcmp(p, "-ip"))
            ip = v;
        else if(!strcmp(p, "-port")){
            port = atoi(v);
        }
        else if(!strcmp(p, "-nc")){
            nc = atoi(v);
            is_nc = 1;
        }
        else if(!strcmp(p, "-path")){
            path = v;
            is_path = 1;   
        }
        else
            printf("Found invalid parameter and Ignored\n");
    }

    if(is_client == 1 &&  is_server == 1){
        printf("Please provide valid input for -s or -c\n");
        exit(1);
    }

    if(is_client == 1 && is_path == 1)
        printf("Path provided is ignored\n");
    
    if(is_client == 1 && is_nc){
        printf("Number of clients provided is ignored\n");
    }

    if(is_server){
        is_server = 1;
        int mat_n = read_mat(path);
        int socketfd = server_init(ip, port, &s_addr);
        serves_clients(socketfd, nc, mat_n, socketfds, c_addr);
        close(socketfd);
    }
    else if (is_client){
        is_client = 1;
        int socketfd = client_connection_init(ip, port, &s_addr);
        close(socketfd);
    }else
        printf("Please provide valid input for -s or -c\nz");
}