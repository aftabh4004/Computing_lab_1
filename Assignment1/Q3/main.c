#include "header.h"

int main(){

    int n = 0, format;
    printf("How many dates? ");
    scanf("%d", &n);
    
    //To read new line character left in STDIN
    fgetc(stdin);

    printf("Enter dates in EU (DD-MMM-YYYY) or US (MMM DD, YYYY) format\n");
    Date* datearr[n];
    int i = 0;
    while(i < n){
        
        char temp[14] = {'\0'};
        fgets(temp, 14, stdin);
         
        datearr[i] = (Date*)malloc(sizeof(Date));
        if(temp[2] == '-' && temp[6] == '-'){
            sscanf(temp, "%2d-%3s-%4d", &datearr[i]->dd, datearr[i]->mmm, &datearr[i]->yyyy);
            datearr[i]->format = 0;
        }else if(temp[3] == ' '  && temp[6] == ',' && temp[7] == ' '){
            sscanf(temp, "%3s %2d, %4d", datearr[i]->mmm, &datearr[i]->dd, &datearr[i]->yyyy);
            datearr[i]->format = 1;
        }
        // printf("%s", datearr[i]->mmm);
        datearr[i]->m_num = months_number(datearr[i]->mmm);

        if(!validate(datearr[i])){
            printf("Invalid input. Try again!\n");
            free(datearr[i]);
            continue;
        }

        i++;
    }

    sort_dates(0, n - 1, datearr);

    printf("\nDates in chronological order\n_________________\n\n");
    for(int i = 0; i < n; i++){
        if(datearr[i]->format == 0)
            printf("%02i-%3s-%04i\n", datearr[i]->dd, datearr[i]->mmm, datearr[i]->yyyy);
        else
            printf("%3s %02i, %04i\n",  datearr[i]->mmm, datearr[i]->dd, datearr[i]->yyyy);
    }
}