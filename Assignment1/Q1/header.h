#include<stdio.h>
#include <stdlib.h>
#define MAX_LEN 100


struct BigInt {
    int digit;
    struct BigInt* next;
};

typedef struct BigInt BigInt;


BigInt* create_list (char* num);
BigInt* big_add(BigInt* a, BigInt* b);
BigInt* big_multiply(BigInt* a, BigInt* b);
void printbi(BigInt* head);
int validate(char* num);