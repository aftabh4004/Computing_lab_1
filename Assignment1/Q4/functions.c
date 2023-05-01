#include "header.h"
// extern int count ;
void print_list(Node* head){
    printf("[");
    while(head != NULL){
        if (head->next != NULL)
            printf("%d, ", head->val);
        else
            printf("%d", head->val);
        head = head->next;
    }
    printf("]\n");
}

void insert_at_end(Node** head, Node** last, Node* node){
    if(!(*head) && !(*last)){
        *head = *last = node;
    }
    else{
        node->next = NULL;
        node->prev = *last;
        (*last)->next = node;
        *last = node;
    }
}

void delete_from_end(Node** head, Node** last){
    Node* temp = *last;
    if(*head == *last)
        *head = *last = NULL;
    else{
        *last = (*last)->prev;
        (*last)->next = NULL;
    }
    
    free(temp);
}

void print_subset_k(int n, int k, int i, int cursize, Node* head, Node* last){
    
    if(cursize == k){
        print_list(head);
        count++;
        return;
    }

    for(; i <= n; i++){
        Node * node = (Node*) malloc(sizeof(Node));
        node->val = i ;
        node->next = NULL;
        node->prev = NULL;

        //Insert at end
        insert_at_end(&head, &last, node);

        print_subset_k(n, k, i + 1, cursize + 1, head, last);

        //Delete from end
        delete_from_end(&head, &last);

    }
}
