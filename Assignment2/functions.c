#include "header.h"

int validate(Dob d){
	int isleap = __isleap(d.yyyy); //non zero if year is leap, defined in time.h
	int max_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int max_days_leap[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	int max_day = isleap != 0 ? max_days_leap[d.mm - 1] : max_days[d.mm - 1]; 

	if(d.mm < 1 || d.mm > 12)
		return 0;

	if(d.dd < 1 || d.dd > max_day)
			return 0;

	return 1;
}


void process_dob(char* dob, Student* s){

	sscanf(dob, "%2d%2d%4d", &(s->dob.dd), &(s->dob.mm), &(s->dob.yyyy));
	if(!validate(s->dob)){
		printf("Invalid Dob for %s\n", s->name);
		exit(1);
	}
	struct tm t = {0, 0, 0, s->dob.dd, s->dob.mm - 1, s->dob.yyyy-1900};
    time_t t_of_day = mktime(&t);


	if(t_of_day == -1)
		printf("failed\n");
	s->dob.ndays = (long) t_of_day/86400;

}

Student* process_input(int id, char* inp){
	Student* s = (Student*)malloc(sizeof(Student));
	s->s_id = id;
	char* inp_arr[5] = {NULL};

	char* token = strtok(inp, " ");
	int i = 0;
	while(token != NULL){
		inp_arr[i++] = token;
		token = strtok(NULL, " ");
	}

	char* temp_name = (char*)malloc(sizeof(char)*NAME_LEN);
	int j = 0;
	i = 1;
	while(inp_arr[i] != NULL){
		int k = 0;
		while((temp_name[j++] = inp_arr[i][k++]) != '\0');
		temp_name[j-1] = ' ';
		i++;
	}
	temp_name[j-2] = '\0';

	
	s->name = temp_name;

	//process dates
	process_dob(inp_arr[0], s);
	

	
	return s;
}


void merge_dobdif(int l, int r, int m, Dob_diff* arr[]){
	Dob_diff* temp[r - l + 1];

	int i = l, j = m + 1, k = 0;

	while (i <= m && j <= r){
		int res = arr[i]->diff > arr[j]->diff ? 1 : 0;
		if(res)
			temp[k++] = arr[j++];
		else
			temp[k++] = arr[i++];
	}

	while(i <= m)
		temp[k++] = arr[i++];

	while(j <= r)
		temp[k++] = arr[j++];

	for(i = l, k = 0; i <= r; i++, k++){
		arr[i] = temp[k];
	}

}

void merge_sort_dobdiff(int l, int r, Dob_diff* arr[]){
	if(l >= r)
		return;
	
	int m = (l + r)/2;
	merge_sort_dobdiff(l, m, arr);
	merge_sort_dobdiff(m + 1, r, arr);
	merge_dobdif(l, r, m, arr);
}


void merge_student(int l, int r, int m, Student* arr[]){
	Student* temp[r - l + 1];

	int i = l, j = m + 1, k = 0;

	while (i <= m && j <= r){
		int res = arr[i]->dob.ndays > arr[j]->dob.ndays ? 1 : 0;
		if(res)
			temp[k++] = arr[j++];
		else
			temp[k++] = arr[i++];
	}

	while(i <= m)
		temp[k++] = arr[i++];

	while(j <= r)
		temp[k++] = arr[j++];

	for(i = l, k = 0; i <= r; i++, k++){
		arr[i] = temp[k];
	}

}

void merge_sort_student(int l, int r, Student* arr[]){
	if(l >= r)
		return;
	
	int m = (l + r)/2;
	merge_sort_student(l, m, arr);
	merge_sort_student(m + 1, r, arr);
	merge_student(l, r, m, arr);
}




void rand_str(char *dest, size_t length) {
    char charset[] = "abcdefghijklmnopqrstuvwxyz";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

int input_generator(int n){
	srand(time(0));
	FILE* fp = fopen("input.txt", "w");
	if(!fp){
		printf("Error in writing input\n");
		exit(1);
	}

	int k = rand() % (n/2) + 1;

	if(fprintf(fp, "%d %d\n", n, k) < 0){
		printf("Error in writing file\n");
		exit(1);
	}
	

	for(int i = 0; i < n; i++){
		int dd = rand() % 28 + 1;
		int mm =  rand() % 12 + 1;
		int yyyy = rand() % 10 + 2001;

		int fname_len = rand() % 4 + 4;
		int lname_len = rand() % 4 + 4;

		char fname[22] = {'\0'}, lname[10] = {'\0'};

		rand_str(fname, fname_len);
		rand_str(lname, lname_len);

		strcat(fname, " ");
		strcat(fname, lname);

		
		if(fprintf(fp, "%02d%02d%04d %s\n", dd, mm, yyyy, fname) < 0){
			printf("Error in writing file\n");
			exit(1);
		}
	}
	fclose(fp);
	
}
