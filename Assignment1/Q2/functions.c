#include "header.h"

void process_line(char* line, StudentRecord* record){
    int i = 0, k = 0;
    //reading student id
    while(line[i] != '\0' && line[i] != ' ')
        record->s_id[k++] = line[i++];

    i++;
    //reading CGPA
    k = 0;
    char temp[5] = {'\0'};
    while(line[i] != '\0' && line[i] != ' ')
        temp[k++] = line[i++];
    
    sscanf(temp, "%lf", &record->CGPA);

    i++;
    //read name
    k = 0;
    while(line[i] != '\0' && line[i] != '\n' && line[i] != '\r')
        record->name[k++] = line[i++];

}

int read_records(FILE* fptr, int n,  StudentRecord* records[n]){
    for(int i = 0; i < n ; i++){
        records[i] = (StudentRecord*)malloc(sizeof(StudentRecord));
        char line[MAX_LEN] = {'\0'};
        if(!feof(fptr)){  
            fgets(line, MAX_LEN, fptr);
            process_line(line, records[i]);

        }
        else{
            printf("Not Enough data.\n");
            return 1;
        }
    }
    return 0;
}

int write_sorted_records(int n, StudentRecord* records[]){
    FILE* fptr = fopen("output.txt", "w");
    if(!fptr){
        return 0;
    }

    fprintf(fptr, "%-25s %-12s %-5s\n", "Name", "Rollno", "CGPA");
    
    for(int i = 0; i < n; i++)
        fprintf(fptr, "%-25s %-12s %0.2lf\n", records[i]->name, records[i]->s_id, records[i]->CGPA);
    
    fclose(fptr);
    return 1;
}

int string_comparator(char* a, char* b){
    int i = 0;
    
    // convert to lowercase
    while (a[i] != '\0'){
        if(a[i] >= 65 && a[i] <= 90)
            a[i] += 32;
        i++;
    }

    i = 0;
    while (b[i] != '\0'){
        if(b[i] >= 65 && b[i] <= 90)
            b[i] += 32;
        i++;
    }

    i = 0;
    while(a[i] != '\0' || b[i] != '\0'){

        if(a[i] > b[i]){
            return 1;
        }
        if (a[i] < b[i]){
            return -1;
        }

        i++;
    }
    return 0;
}

int name_comparator(char* a, char* b){
    char afirst[MAX_LEN], alast[MAX_LEN], bfirst[MAX_LEN], blast[MAX_LEN];

    sscanf(a, "%s %s", afirst, alast);
    sscanf(b, "%s %s", bfirst, blast);


    //First name comparison
    int fname_res = string_comparator(afirst, bfirst);
    int lname_res;
    if(fname_res == 0){
        lname_res = string_comparator(alast, blast);
        return lname_res;   
    }
    return fname_res;
}

void merge_records(int l, int r, int m, StudentRecord* arr[]){
    StudentRecord* temp[r - l + 1];
    int i = l, j = m + 1, k = 0;

    while (i <= m  && j <= r){
        int res = name_comparator(arr[i]->name, arr[j]->name);
        if(res == -1)
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }

    while(i <= m)
        temp[k++] = arr[i++];
    
    while(j <= r)
        temp[k++] = arr[j++];

    for(int i = l, k = 0; i <= r; i++, k++)
        arr[i] = temp[k];
}

void sort_records(int l, int r, StudentRecord* arr[]){
    if(l >= r)
        return;

    int m = (l + r)/2;
    sort_records(l, m, arr);
    sort_records(m + 1, r, arr);

    merge_records(l, r, m, arr);
}