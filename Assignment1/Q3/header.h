#include<stdio.h>
#include<stdlib.h>

struct Date
{
    int dd;
    char mmm[4];
    int yyyy;
    int m_num;
    int format;
};

typedef struct Date Date;

int months_number(char* month);
int date_comparator(Date *a, Date *b);
void merge_dates(int l, int r, int m, Date* arr[]);
void sort_dates(int l, int r,  Date* arr[]);
int leap(int y);
int validate(Date* d);