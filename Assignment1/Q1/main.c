#include "header.h"


int main(){
   char num1[MAX_LEN] = {'\0'}, num2[MAX_LEN] = {'\0'};
   int t;
   printf("Enter number of test cases: ");
   scanf("%d", &t);

   printf("Input format\nAdd: 0 num1 num2\nMul: 1 num1 num2\n");
  

   while(t--){
      printf("Please provide input\n");
      int operation;
      scanf("%d %s %s", &operation, num1, num2);

      if(validate(num1) || validate(num2)){
         printf("Invalid input. Try again!\n");
         t++;
         continue;
      }

      BigInt* a = create_list(num1);
      BigInt* b = create_list(num2);

      BigInt* ans;

      if(operation == 0)
         ans = big_add(a, b);
      else if (operation == 1)
         ans = big_multiply(a, b);
      
      printbi(ans);
   }
   


}