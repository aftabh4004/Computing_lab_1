#include "header.h"


int main(int argc, char const *argv[])
{
	const char* path, *algo;

	if(argc != 3){
		printf("%s\n", "Input missing");
		return 1;
	}

	path = argv[2];
	algo = argv[1];

	clock_t start_time = clock();

	FILE* fptr = fopen(path, "r");
	if(!fptr){
		printf("%s\n", "Error in opening input file");
		perror("Error");
		return 1;
	}


	int n, k;
	char t;
	fscanf(fptr, "%d %d", &n, &k);
	fgetc(fptr);


	if(k > n/2){
		printf("K must be less than floor of n/2\n");
		exit(1);
	}
	
	Student* sarr[n];

	for(int i = 0; i < n; i++){
		char temp[MAX_LEN];
		fgets(temp, MAX_LEN, fptr);
		
		sarr[i] = process_input(i, temp);
	}

	if(!strcmp(algo, "a"))
		algo_a(n, k, sarr);
	else if(!strcmp(algo, "b"))
		algo_b(n, k, sarr);
	else if(!strcmp(algo, "c"))
		algo_c(n, k, sarr);
	else{
		printf("Invalid algo choice.\n");
		exit(1);
	}


	clock_t end_time = clock();
	clock_t time_diff = end_time -  start_time;
	double tf = (double)time_diff/CLOCKS_PER_SEC;

	FILE* ftime = fopen("plottime.csv", "a");
	if(!ftime){
		printf("%s\n", "Error in opening plot time file");
		perror("Error");
		return 1;
	}

	if(fprintf(ftime, "%d,%d,%s,%lf\n", n, k, algo, tf) < 0){
		printf("Error[1] in writing time in file\n");
		exit(1);
    }

	fclose(ftime);
	printf("Clock time taken by the program: %lf\n", tf);
	return 0;
}