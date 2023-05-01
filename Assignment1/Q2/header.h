#include<stdio.h>
#include<stdlib.h>
#define MAX_LEN 100

struct StudentRecord{
    char name[MAX_LEN];
    char s_id[10];
    double CGPA;
};

typedef struct StudentRecord StudentRecord;


void process_line(char* line, StudentRecord* record);
int read_records(FILE* fptr, int n,  StudentRecord* records[n]);
int write_sorted_records(int n, StudentRecord* records[]);
int string_comparator(char* a, char* b);
int name_comparator(char* a, char* b);
void merge_records(int l, int r, int m, StudentRecord* arr[]);
void sort_records(int l, int r, StudentRecord* arr[]);