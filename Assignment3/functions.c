#include "header.h"


/*

@function   read_input()
@discussion     Function to read input from the file and store in the crated data structure.   
@param     fp: File pointer to the input file.
@param     n: number of stages
@param     prob[]: array to store probabilities
@param     prod_cost[]: array to store product cost
@param     *f[]: array to store the fixed charge of inspection
@param     *g[]: array to store the variable charge of inspection

*/
void read_input(FILE* fp, int n, double prob[], double prod_cost[], double* f[], double* g[]){
    for(int i = 0; i < n; i++){
        fscanf(fp, "%lf,", &prob[i]);
        fscanf(fp, "%lf,", &prod_cost[i]);
        
        if(prob[i] < 0 || prob[i] > 1){
            printf("Invalid probability for node %d\n", i + 1);
            exit(1);
        }

        if(prod_cost[i] < 0){
            printf("Invalid(Negative) production cost for node %d\n", i + 1);
            exit(1);
        }

        f[i] = (double*)malloc(sizeof(double) * (i +1));
        for(int j = 0; j < (i +1); j++){
            fscanf(fp, "%lf,", &f[i][j]);
            if(f[i][j] < 0){
                printf("Invalid(Negative) fixed cost for nodes %d %d\n",i,  j + 1);
               exit(1);
            }
        }
        
        g[i] = (double*)malloc(sizeof(double) * (i +1));
        for(int j = 0; j < (i +1); j++){
            j == i ? fscanf(fp, "%lf", &g[i][j]): fscanf(fp, "%lf,", &g[i][j]); 
            if(g[i][j] < 0){
                printf("Invalid(Negative) variable cost for nodes %d %d\n", i, j + 1);
                exit(1);
            }
        }
    }
}



/*

@function   build_graph()
@discussion     Calculating the weight of all the edges and store in the graph data structure.   
@param     n: number of stages
@param     graph[][]: data structure for the graph
@param     gp[]: array of good products at every stage
@param     presum_pdc[]: presum array of the production cost to get the sum of production cost between any two stage
@param     *f[]: array of fixed charge of inspection
@param     *g[]: array of variable charge of inspection

*/
void build_graph(int n, int b, double graph[][n + 1], double gp[],  double presum_pdc[], double* f[], double* g[]){
    for(int i = 0; i < n + 1; i++)
        for(int j = 0; j < n + 1; j++)
            graph[i][j] = 0;
    
    for(int i = 0; i < n; i++){
        for(int j = i + 1; j < n + 1; j++){
            graph[i][j] = (presum_pdc[j] - presum_pdc[i]) * gp[i] + f[j - 1][i] + g[j - 1][i] * gp[i];
        }
    }
}



/*

@function   find_min()
@discussion     Function to find the node with the smallest distance from the source, as of now, during the intermidiate step of dijktra algo.   
@param     n: number of stages
@param     arr[]: distance array of intermediate graph
@param     processed[]: array that contain the nodes that are already being processed

*/
int find_min(int n, double arr[], int processed[]){
    
    int min = MAX_VAL, mini;
    for(int i = 0; i < n; i++){

        // If the node is already processed, skip that node in finding min.
        if(processed[i])
            continue;
        
        if(arr[i] < min){
            mini = i;
            min = arr[i];
        }
    }
    return mini;
}



/*

@function   dijkstra_shortest_path()
@discussion     Implementation of dijkstra algo not using heap because the graph is dense   
@param     n: number of stages
@param     graph[][]: graph data structure

*/

int* dijkstra_shortest_path(int n, double graph[][n]){
    double dist[n];
    int processed[n];
    int* parent = (int*)malloc(sizeof(int) * n);

    // Initializing 
    for(int i = 0; i < n; i++) {
        parent[i] = -1;
        dist[i] = MAX_VAL;
        processed[i] = 0;
    }
    

    // start with the node 0
    dist[0] = 0;



    for(int i = 0; i < n; i++){
        int min = find_min(n, dist, processed);
        processed[min] = 1;
        


        //Relaxing egde if the distance of node improves by introducing ith node as intermediate node
        for(int j = 0; j < n; j++){
            if(graph[min][j] != 0 && dist[min] + graph[min][j] < dist[j]){
                dist[j] = dist[min] + graph[min][j];
                parent[j] = min;

            }
        }
    }
    printf("Total cost: %.2lf\n",dist[n-1]);

    return parent;
}