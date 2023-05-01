#include "header.h"

int main(){
    FILE *fptr;
    fptr = fopen("input.txt", "r");

    if(!fptr){
        printf("Error in opening the file\n");
        return 1;
    }
    int n;
    fscanf(fptr, "%d\n", &n);
    StudentRecord* records[n];

    read_records(fptr, n, records);
    fclose(fptr);
    

    sort_records(0, n-1, records);

    if(!write_sorted_records(n, records))
        printf("Error in writing file\n");
    else
        printf("File written successfully\n");

}