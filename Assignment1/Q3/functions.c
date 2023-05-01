#include "header.h"

int leap(int y){
    if(y % 400 == 0)
        return 1;
    if(y % 100 == 0)
        return 0;
    if(y % 4 == 0)
        return 1;
    return 0;
}


int validate(Date* d){
    int max_days = 0;
    switch (d->m_num){
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12: max_days = 31; break;
    case 4:
    case 6:
    case 9:
    case 11: max_days = 30; break;
    case 2:
            if(leap(d->yyyy))
                max_days = 29;
            else
                max_days = 28;
            break;
    
    default:
        // Invalid month
        // printf("Invalid month %d\n", d->m_num);
        return 0;
    }
    if(d->dd <= 0 || d->dd > max_days){
        // printf("Invalid days\n");
        return 0;
    }

    

    // valid date
    
    return 1;
}

int months_number(char* month){
    char* months[] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};
    
    char lc_month[5] = {'\0'};

    //to lowercase
    int i = 0;
    while (month[i] != '\0' && month[i] != '\r' && month[i] != '\n'){
        if(month[i] >= 'A' && month[i] <= 'Z')
            lc_month[i] = month[i] + ('a' - 'A');
        else
            lc_month[i] = month[i];
        i++;
    }
    

    //comparison
    for(int i = 0; i < 12; i++){
        if(lc_month[0] == months[i][0] && lc_month[1] == months[i][1] && lc_month[2] == months[i][2]){
            return i + 1;
        }
    }
    return -1;
}

int date_comparator(Date *a, Date *b){
    
    if (a->yyyy > b->yyyy)
        return 1;
    if(a->yyyy < b->yyyy)
        return -1;
    
    if(a->m_num > b->m_num)
        return 1;
    if(a->m_num < b->m_num)
        return -1;
    
    if(a->dd > b->dd)
        return 1;
    if(a->dd < b->dd)
        return -1;
    
    return 0;
}

void merge_dates(int l, int r, int m, Date* arr[]){
    int n = r - l + 1;
    Date* temp[n];
    int i = l, j = m + 1, k = 0;

    while(i <= m && j <= r){
        int res = date_comparator(arr[i], arr[j]);

        if(res == -1){
            temp[k++] = arr[i];
            i++;
        }
        else{
            temp[k++] = arr[j];
            j++;
        }
    }

    while (i <= m){
        temp[k++] = arr[i++];
    }

    while (j <= r){
        temp[k++] = arr[j++];
    }

    k = 0;
    for(int i = l; i <= r; i++){
        arr[i] = temp[k++];
    }
}

void sort_dates(int l, int r,  Date* arr[]){
    if(l >= r)
        return;

    int m = (l + r)/2;
    sort_dates(l, m, arr);
    sort_dates(m + 1, r, arr);

    merge_dates(l, r, m, arr);
}
