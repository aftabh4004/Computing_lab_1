#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define NAME_LEN 20
#define PATH_LEN 100
#define MAX_LEN 30

struct Dob{
	int dd;
	int mm;
	int yyyy;
	int ndays;
};

typedef struct Dob Dob;

struct Student {
	int s_id;
	char* name;
	Dob dob;
};

typedef struct Student Student;

struct Dob_diff {
	Student* s1;
	Student* s2;
	int diff;
};
typedef struct Dob_diff Dob_diff;

struct Heap_dobdiff {
	int size;
	Dob_diff** harr;
};
typedef struct Heap_dobdiff Heap_dobdiff;


Student* process_input(int, char*);
void merge_sort_dobdiff(int l, int r, Dob_diff* arr[]);
void merge_sort_student(int l, int r, Student* arr[]);

void algo_a(int n, int k, Student* sarr[]);
void algo_b(int n, int k, Student* sarr[]);
void algo_c(int n, int k, Student* sarr[]);


void heapify(int i, Heap_dobdiff* heap);
Dob_diff* pop_min(Heap_dobdiff* heap);


int input_generator(int n);