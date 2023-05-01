#include<stdio.h>
#include<stdlib.h>

#define MAX_VAL 999999999
#define MAX_LEN 100

void read_input(FILE*, int n, double prob[], double prod_cost[], double* f[], double* g[]);
void build_graph(int n, int b, double graph[][n + 1], double gp[], double presum_pdc[], double* f[], double* g[]);
int* dijkstra_shortest_path(int n, double graph[][n]);
