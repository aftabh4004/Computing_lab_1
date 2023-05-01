#include<stdio.h>
#include<stdlib.h>
typedef struct Node
{
    int val;
    struct Node* next;
    struct Node* prev ;
} Node;

// typedef struct Node Node;

extern int count;
void print_list(Node* head);
void insert_at_end(Node** head, Node** last, Node* node);
void delete_from_end(Node** head, Node** last);
void print_subset_k(int n, int k, int i, int cursize, Node* head, Node* last);