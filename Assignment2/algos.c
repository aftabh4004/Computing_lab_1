#include "header.h"

void algo_a(int n, int k, Student* sarr[]){
    int m = (n * (n - 1))/2, p = 0;

    Dob_diff* dobdiff[m];
    Student* scopy[n];
    for(int i = 0; i < n; i++) scopy[i] = sarr[i];

    for(int i = 0; i < n; i++){
        for (int j = i + 1; j < n; j++){
            dobdiff[p] = (Dob_diff*)malloc(sizeof(Dob_diff));
            dobdiff[p]->diff = abs(sarr[i]->dob.ndays - sarr[j]->dob.ndays);
            dobdiff[p]->s1 = sarr[i];
            dobdiff[p]->s2 = sarr[j];
            p++;
        }
    }

    merge_sort_dobdiff(0, m - 1, dobdiff);

    Student* left = NULL;
    int flag[n], count = 0;
    for (int i = 0; i < n; i++) flag[i] = 0;

    
    FILE *fptr = fopen("output1.txt", "w");

    if(!fptr){
        printf("Error[1] in opening  output of algo a in file\n");
        exit(1);
    }
    for(int i = 0; i < m; i++){

        if(count == k)
            break;

        if(!flag[dobdiff[i]->s1->s_id]  && !flag[dobdiff[i]->s2->s_id]){
            flag[dobdiff[i]->s1->s_id] = 1;
            flag[dobdiff[i]->s2->s_id] = 1;

            if(fprintf(fptr, "%-15s %-15s %d\n", dobdiff[i]->s1->name, dobdiff[i]->s2->name, dobdiff[i]->diff) < 0){
                printf("Error[2] in writing output of algo a in file\n");
                exit(1);
            }
            count++;

        }
    }

    
    for(int i = 0; i < n ; i++){
        if(flag[i] == 0){
            if(fprintf(fptr, "%-15s %d\n", scopy[i]->name, 0) < 0){
                printf("Error[3] in writing output of algo a in file\n");
                exit(1);
            }
        }
    }


    fclose(fptr);
    printf("output1.txt file written successfully!\n");
    
}


void algo_b(int n, int k, Student* sarr[]){
    Student* scopy[n];
    for(int i = 0; i < n; i++) scopy[i] = sarr[i];

    Dob_diff* dobdiff[n - 1];
    merge_sort_student(0, n - 1, sarr);


    for(int i = 0; i < n - 1; i++){
        dobdiff[i] = (Dob_diff*)malloc(sizeof(Dob_diff));
        dobdiff[i]->diff = abs(sarr[i]->dob.ndays - sarr[i + 1]->dob.ndays);
        dobdiff[i]->s1 = sarr[i];
        dobdiff[i]->s2 = sarr[i + 1];
    }

    merge_sort_dobdiff(0, n - 2, dobdiff);


    int flag[n], count = 0;
    for (int i = 0; i < n; i++) flag[i] = 0;

    
    FILE *fptr = fopen("output2.txt", "w");

    if(!fptr){
        printf("Error in opening output 2 file\n");
        exit(1);
    }
    for(int i = 0; i < n - 1; i++){

        if(count == k)
            break;

        if(!flag[dobdiff[i]->s1->s_id]  && !flag[dobdiff[i]->s2->s_id]){
            flag[dobdiff[i]->s1->s_id] = 1;
            flag[dobdiff[i]->s2->s_id] = 1;

            if(fprintf(fptr, "%-15s %-15s %d\n", dobdiff[i]->s1->name, dobdiff[i]->s2->name, dobdiff[i]->diff) < 0){
                printf("Error[1] in writing output of algo b in file\n");
                exit(1);
            }
            count++;

        }
    }

    
    
    for(int i = 0; i < n ; i++){
        if(flag[i] == 0){
            if(fprintf(fptr, "%-15s %d\n", scopy[i]->name, 0) < 0){
                printf("Error[3] in writing output of algo b in file\n");
                exit(1);
            }
        }
    }

    if(fprintf(fptr, "Highest number of disjoint groups possible is %d\n", count) < 0){
        printf("Error[2] in writing output of algo b in file\n");
        exit(1);
    }


    fclose(fptr);
    printf("output2.txt file written successfully!\n");

}


void algo_c(int n, int k, Student* sarr[]){
    Student* scopy[n];
    for(int i = 0; i < n; i++) scopy[i] = sarr[i];

    merge_sort_student(0, n - 1, sarr);

    int m = n - 1;
    Dob_diff* dobdiff[m + 1];
    dobdiff[0] = NULL;  
    for(int i = 1; i <= m; i++){
        dobdiff[i] = (Dob_diff*)malloc(sizeof(Dob_diff));
        dobdiff[i]->diff = abs(sarr[i]->dob.ndays - sarr[i - 1]->dob.ndays);
        dobdiff[i]->s1 = sarr[i];
        dobdiff[i]->s2 = sarr[i - 1];
    }


    Heap_dobdiff* heap = (Heap_dobdiff*)malloc(sizeof(Heap_dobdiff));
    heap->harr = dobdiff;
    heap->size = m;

    for(int i = m/2; i >= 1; i--)
        heapify(i, heap);


    int flag[n], count = 0;
    for (int i = 0; i < n; i++) flag[i] = 0;

    
    FILE *fptr = fopen("output3.txt", "w");

    if(!fptr){
        printf("Error in opening output 3 file\n");
        exit(1);
    }

    for(int i = 1; i <= m; i++){

        Dob_diff* min = pop_min(heap);
        
        if(count == k)
            break;

        if(!flag[min->s1->s_id]  && !flag[min->s2->s_id]){
            flag[min->s1->s_id] = 1;
            flag[min->s2->s_id] = 1;

            if(fprintf(fptr, "%-15s %-15s %d\n", min->s2->name, min->s1->name, min->diff) < 0){
                printf("Error[1] in writing output of algo b in file\n");
                exit(1);
            }
            count++;

        }
    }

    
    
    for(int i = 0; i < n ; i++){
        if(flag[i] == 0){
            if(fprintf(fptr, "%-15s %d\n", scopy[i]->name, 0) < 0){
                printf("Error[3] in writing output of algo c in file\n");
                exit(1);
            }
        }
    }

    if(fprintf(fptr, "Highest number of disjoint groups possible is %d\n", count) < 0){
        printf("Error[2] in writing output of algo c in file\n");
        exit(1);
    }


    fclose(fptr);
    printf("output3.txt file written successfully!\n");
}