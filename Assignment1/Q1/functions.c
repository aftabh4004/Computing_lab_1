#include "header.h"


int validate(char* num){
    int i = 0;
    while (num[i] != '\0'){
        if(num[i] - '0' > 9 || num[i] - '0' < 0)
            return 1;
        i++;
    }
    return 0;
    
}

BigInt* reverse_list(BigInt* head){
    if (!head || !head->next)
        return head;
    
    if(!head->next->next){
        BigInt* t = head->next;
        t->next = head;
        head->next = NULL;
        return t;
    }

    BigInt* prev = NULL, *cur = head, *nxt = head;

    while (cur){
        nxt = nxt->next;
        cur->next = prev;
        prev = cur;
        cur = nxt;
    }

    return prev;
}

void printbi(BigInt* head){
    printf("Answer: ");
    BigInt* cur = head;
    while (cur != NULL){
        printf("%d", cur->digit);
        cur = cur->next;
    }
    printf("\n");
}

BigInt* add_node_front(BigInt* head, int val){
    BigInt* node = (BigInt*)malloc(sizeof(BigInt));
    node->digit = val;
    node->next = NULL;


    if(head == NULL)
        head = node;
    else{
        node->next = head;
        head = node;
    }
    return head;
}

BigInt* create_list (char* num){
    BigInt* head = NULL;
    int i = 0;
    while(num[i] != '\0'){
        head = add_node_front(head, num[i] - 48);
        i++;
    }
    return head;
}

BigInt* big_add(BigInt* a, BigInt* b){
    
    BigInt* c = NULL, *cura = a, *curb = b;
    int carry = 0;
    while(cura != NULL || curb != NULL){
        int aval, bval;
        if(cura != NULL){
            aval = cura->digit;
            cura = cura->next;
        }
        else
            aval = 0;

        if(curb != NULL){
            bval = curb->digit;
            curb = curb->next;
        }
        else
            bval = 0;

        int sum = aval + bval + carry;
        carry = sum / 10;
        sum = sum % 10;
      
        c = add_node_front(c, sum);

        
    }
    if(carry != 0){
        BigInt* node = (BigInt*)malloc(sizeof(BigInt));
        node->digit = carry;

        node->next = c;
        c = node;
    }
    return c;
}


BigInt* big_multiply(BigInt* a, BigInt* b){
    BigInt* curb = b, *cura, *c = NULL, *prev_temp = NULL;
    int shift = 0;
    while(curb){
        int bval = curb->digit, carry = 0;
        curb = curb->next;
        
        BigInt* cur_temp = NULL;

        for (int i = 0; i < shift; i++){
            cur_temp = add_node_front(cur_temp, 0);
        }
        
        cura = a;
        while (cura){
            int aval = cura->digit;
            cura = cura->next;

            int mul = aval * bval + carry;
            carry = mul / 10;
            mul = mul % 10;

            cur_temp = add_node_front(cur_temp, mul);
            

        }
        if(carry != 0)
            cur_temp = add_node_front(cur_temp, carry);
        
        cur_temp = reverse_list(cur_temp);
        prev_temp = reverse_list(prev_temp);
        prev_temp = big_add(cur_temp, prev_temp);


        //free cur temp

        
        shift++;
        
    }
    return prev_temp;
}