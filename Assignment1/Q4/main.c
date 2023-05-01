#include "header.h"

int count = 0;
int main(){
    int n, k;

    printf("Enter n: ");
    scanf("%d", &n);
    printf("Enter k: ");
    scanf("%d", &k);
    
    print_subset_k(n, k, 1, 0, NULL, NULL);
    printf("Total number of subsets: %d\n", count);
}