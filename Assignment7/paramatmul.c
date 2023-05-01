#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<pthread.h>
#include<unistd.h>
#define MAXNUMMAT 1000

typedef struct MAT {
    long long int **mat;
    int r, c;
} MAT;



MAT* A[MAXNUMMAT];
MAT* B[MAXNUMMAT];
int blen;
int aready;
int done;


//pthread entity
pthread_mutex_t mutexblen;
pthread_mutex_t mutexdone;
pthread_barrier_t barrierBready;
pthread_barrier_t barrierAready;


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
    for(int i = 0; i < m->r; i++){
        free(m->mat[i]);
    }
}



void duplicate_mat(MAT *a, MAT *c){   
    for (int i = 0; i < a->r; i++){
        for(int j = 0; j < a->c; j++){
            c->mat[i][j] = a->mat[i][j];
        }
    }
}

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


void* matmulhandler(void* arg){
    int i = *(int*) arg;
    pthread_mutex_lock(&mutexdone);
    while(!done){
        pthread_mutex_unlock(&mutexdone);
        // printf("thread waiting for A ready %ld\n", pthread_self());
        pthread_barrier_wait(&barrierAready);
        if(A[i] != NULL){
            MAT *a, *b, *c;   
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
    
            B[i/2] = c;

            // printf("th %ld blen %d\n", pthread_self(), blen);
            pthread_mutex_lock(&mutexblen);
            blen++;
            pthread_mutex_unlock(&mutexblen);
        }
        // printf("thread waiting for B ready %ld\n", pthread_self());
        pthread_barrier_wait(&barrierBready);
    }

    pthread_mutex_unlock(&mutexdone);
}

int main(){
    int alen, mat_n;
    FILE* fp = fopen("input.txt", "r");
    if(!fp){
        printf("Unable to open input file\n");
        exit(1);
    }

    fscanf(fp, "%d", &mat_n);

    for(int i = 0; i < mat_n; i++){
        int r, c;
        fscanf(fp, "%d %d",&r, &c);
        MAT* tm = create_mat(r, c);
        for(int j = 0; j < r; j++){
            for(int k = 0; k < c; k++){
                fscanf(fp, "%lld", &tm->mat[j][k]);
            }
        }
        A[i] = tm;
    }
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



    // printf("num t %d alen %d\n", num_t, alen);
    // Creating threads

    for(int i = 0; i < num_t; i++){
        int *indx = (int*) malloc(sizeof(int));
        *indx = 2 * i;
        if (pthread_create(&th[i], NULL, matmulhandler, indx)){
            printf("Unable to create thread\n");
            exit(1);
        }

    }
    // printf("main alen %d, Waiting for A\n", alen);
    pthread_barrier_wait(&barrierAready);
    while(alen > 1){
        // printf("main alen %d, Waiting for B\n", alen); 
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
        // printf("main alen %d, blen %d Waiting for A\n", alen, blen);
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

    
    MAT* ans = A[0];
    printf("%d %d\n", ans->r, ans->c);
    for(int i = 0; i < ans->r ; i++){
        for(int j = 0; j < ans->c; j++){
            printf("%10lld ", ans->mat[i][j]);
        }
        printf("\n");
    }

    pthread_mutex_destroy(&mutexblen);
    pthread_mutex_destroy(&mutexdone);
    pthread_barrier_destroy(&barrierBready);
    pthread_barrier_destroy(&barrierAready);

}