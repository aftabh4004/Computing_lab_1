#include "header.h"

int main(int argc, char* argv[]){

    const char * fpath = "input.txt";
    if(argc == 2)
        fpath = argv[1];

    // opening the file provided by the user or input.txt by default
    FILE* fp = fopen(fpath, "r");
    if(!fp){
        printf("Error[1] in opening input file\n");
        exit(1);
    }

    //Reading number of stage and batch size from fiie.
    int n, b = 0;
    char temp[100], temp2[100] = {'\0'};
    fscanf(fp, "%d\n", &n);

    if(n <= 0){
        printf("Invalid value for 'n'\n");
        exit(1);
    }


    printf("Enter the initial batch size: ");
    scanf("%s", temp);
    sscanf(temp, "%d%s", &b, temp2);

    if(temp2[0] != '\0' || b <= 0){
        printf("B must be a non zero positive integer\n");
        exit(1);
    }


    double prob[n];
    double prod_cost[n], *f[n], *g[n], presum_pdc[n + 1], graph[n + 1][n + 1], gp[n + 1];

    read_input(fp, n, prob, prod_cost, f, g);
    fclose(fp);

    //computing pre sum for production cost
    presum_pdc[0] = 0;
    for(int i = 1; i < n + 1; i++){
        presum_pdc[i] = presum_pdc[i - 1] + prod_cost[i-1];
    }

    // Computing good products at every stage
    gp[0] = b;
    for(int i = 1; i < n + 1; i++){
        gp[i] = gp[i - 1] * (1 - prob[i-1]);
    }


    // Stop the program if no good product will come out from the last stage.
    if(gp[n] < 1){
        printf("No good product can be produce.\n");
        exit(1);
    }


    // Calculating the edge weight and putting in the graph.
    build_graph(n, b, graph, gp, presum_pdc, f, g);

    
    // Calling shortest path algo on the graph created
    int* path = dijkstra_shortest_path(n + 1, graph);


    for(int i = 0; i < n + 1; i++){
        for (size_t j = 0; j < n + 1; j++){
            printf("%10.2lf ", graph[i][j]);
        }
        printf("\n");
    }


    // Printing the results by tracing the shortest path from last node to first.
    printf("Inspection on the state: \n");

    int output[MAX_LEN] = {-1};
    int parent = path[n], i = 0, k = 0;
    output[k++] = n;
    while (parent != 0){
        output[k++] = parent;
        parent = path[parent];
    }

    for(int j = k - 1; j >= 0; j--)
        printf("%d ", output[j]);
    printf("\n");
    
}