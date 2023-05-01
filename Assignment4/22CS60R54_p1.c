#include <stdio.h> /* C input/output */
#include <stdlib.h> /* C standard library */
#include <string.h>
#include <glpk.h> /* GNU GLPK linear/mixed integer solver */
#define LEN 10
#define STR_LEN 100


/*
 * Function to convert a string to integer
 * Throws error if the string contain char other then digits
 * @params str: string that has to converted.
*/
int stoi(char* str){
    int i = 0, res = 0;

    while(str[i] != '\0' && str[i]  != '\r' && str[i] != '\n' ){
        if(str[i] < '0' || str[i] > '9'){
            printf("Only positive integer values are allowed!\n");
            exit(1);
        }
        //converting digit into integer
        res = res * 10 + str[i] - '0';
        i++;
    }
    return res;
}


/*
 * Function to read inputs from the file.
 * Throws error and exit the program if find any invalid input.
 * @params fp: file pointer
 * @params m : number of jobs
 * @params D[]: array of duration of the jobs
 * @params f[]: Dependency array of jobs
*/
void read_input(FILE* fp, int m, double D[], int f[][m]){

    //reading the durations
    for(int i = 1; i <= m; i++){
        fscanf(fp, "%lf", &D[i]);
        if(D[i] <= 0){
            printf("Job duration must be positve integer\n");
            exit(1);
        }
    }

    //to read the newline char left in the file by the fscanf
    fscanf(fp, "\n");

    //Read dependencies of the m jobs
    for(int i = 1; i <= m; i++){
        char temp[STR_LEN];
        fgets(temp, STR_LEN, fp);
        char* token = strtok(temp, " ");
        int job = stoi(token);  
        int j = 0;
        token = strtok(NULL, " ");

        while(token != NULL){
            int idash = stoi(token);
            f[job][j] = idash;
            token = strtok(NULL, " ");
            j++;
        }
    }
}


int main(){

    FILE *fp = fopen("input1_2.txt", "r");
    if(!fp){
        printf("Error in opening file.\n");
        exit(1);
    }

    glp_prob *lp;

    int m;
    fscanf(fp, "%d", &m);
    if(m <= 0){
        printf("Number of jobs must be a positive integer\n");
        exit(1);
    }

    double D[m + 1];
    int f[m + 1][m];

    for(int i = 0; i < m + 1; i++){
        D[i] = -1;
        for(int j = 0; j < m; j++)
            f[i][j] = -1;
    }

    read_input(fp, m, D, f);
    fclose(fp);


    // Counting the total number of dependencies to calculate the number of row in the constraint
    int ndep = 0;
    for (size_t i = 0; i < m + 1; i++){
        for (size_t j = 0; j < m; j++){
            if(f[i][j] != -1)
                ndep++;
        }
    }


    // number of row and col is calculated by oberserving the constraint
    int rows  = 9 * m * m - m + ndep * m, cols = 4*m*m + m;
    int M  = 0;


    // Assining M the sum of all D's, so that M can have the value greater than all possible value of xj
    for(int i = 1; i <=m ;i++){
        if (M < D[i])
            M = D[i];
    }
   
    //Heaping array to create rows of constraint
    int constr[rows + 1][cols + 1];

    //initializing constr array
    for (size_t i = 0; i < rows + 1; i++){
        for (size_t j = 0; j < cols + 1; j++){
            constr[i][j] = 0;
        }
    }


    // Storing the base index of u, v, y, z, and x for future use.
    int row = 1, ubase = 1, vbase = m * m  + ubase, ybase = m * m + vbase, zbase = m * m + ybase, xbase = m * m + zbase;



    /* create problem */
    lp = glp_create_prob();
    glp_set_prob_name(lp, "short");
    glp_set_obj_dir(lp, GLP_MIN);

    glp_add_rows(lp, rows);



    //filling constraint in the 2D array
    for(int i = 1; i <= m; i++){

        
        //uij+1 - uij >= 0
        for(int j = 0; j < m - 1; j++){
            char name[LEN];
            sprintf(name, "row%d", row); 
            constr[row][ubase + (i - 1) * m + j] = -1;
            constr[row][ubase + (i - 1) * m + j + 1] = 1;
            glp_set_row_name(lp, row, name);
            glp_set_row_bnds(lp, row, GLP_LO, 0.0, 0.0);
            row++;
        }

        //vij+1 - vij >= 0
        for(int j = 0; j < m - 1; j++){
            char name[LEN];
            sprintf(name, "row%d", row);
            constr[row][vbase + (i - 1) * m + j] = -1;
            constr[row][vbase + (i - 1) * m + j + 1] = 1;
            glp_set_row_name(lp, row, name);
            glp_set_row_bnds(lp, row, GLP_LO, 0.0, 0.0);
            row++;
        }


        //uij - vij >= 0
        for(int j = 0; j < m; j++){
            char name[LEN];
            sprintf(name, "row%d", row);
            constr[row][ubase + (i - 1) * m +  j] = 1;
            constr[row][vbase + (i - 1) * m + j] = -1;
            glp_set_row_name(lp, row, name);
            glp_set_row_bnds(lp, row, GLP_LO, 0.0, 0.0);
            row++;
        }

        //vi'j - uij >= 0
        for(int k = 0; k < m; k++){
            if(f[i][k] != -1){
                int idash = f[i][k];
                for(int j = 0; j < m; j++){
                    char name[LEN];
                    sprintf(name, "row%d", row);
                    constr[row][ubase + (i - 1) * m +  j] = -1;
                    constr[row][vbase + (idash - 1) * m + j] = 1;
                    glp_set_row_name(lp, row, name);
                    glp_set_row_bnds(lp, row, GLP_LO, 0.0, 0.0);
                    row++;
                }

            }
        }


        //Muij - yij >= 0
        for(int j = 0; j < m; j++){
            char name[LEN];
            sprintf(name, "row%d", row);
            constr[row][ubase + (i - 1) * m +  j] = M;
            constr[row][ybase + (i - 1) * m + j] = -1;
            glp_set_row_name(lp, row, name);
            glp_set_row_bnds(lp, row, GLP_LO, 0.0, 0.0);
            row++;
        }

        //xj - yij >= 0
        for(int j = 0; j < m; j++){
            char name[LEN];
            sprintf(name, "row%d", row);
            constr[row][xbase +  j] = 1;
            constr[row][ybase + (i - 1) * m + j] = -1;
            glp_set_row_name(lp, row, name);
            glp_set_row_bnds(lp, row, GLP_LO, 0.0, 0.0);
            row++;
        }

        //yij - xj + (1 - uij)M >= 0
        for(int j = 0; j < m; j++){
            char name[LEN];
            sprintf(name, "row%d", row);
            constr[row][ybase + (i - 1) * m + j] = 1;
            constr[row][xbase +  j] = -1;
            constr[row][ubase + (i - 1) * m + j] = -M;
            glp_set_row_name(lp, row, name);
            glp_set_row_bnds(lp, row, GLP_LO, -M * 1.0, 0.0);
            row++;
        }

        //Muij - zij >= 0
        for(int j = 0; j < m; j++){
            char name[LEN];
            sprintf(name, "row%d", row);
            constr[row][ubase + (i - 1) * m +  j] = M;
            constr[row][zbase + (i - 1) * m + j] = -1;
            glp_set_row_name(lp, row, name);
            glp_set_row_bnds(lp, row, GLP_LO, 0.0, 0.0);
            row++;
        }

        //xj - zij >= 0
        for(int j = 0; j < m; j++){
            char name[LEN];
            sprintf(name, "row%d", row);
            constr[row][xbase +  j] = 1;
            constr[row][zbase + (i - 1) * m + j] = -1;
            glp_set_row_name(lp, row, name);
            glp_set_row_bnds(lp, row, GLP_LO, 0.0, 0.0);
            row++;
        }

        //zij - xj + (1 - vij)M >= 0
        for(int j = 0; j < m; j++){
            char name[LEN];
            sprintf(name, "row%d", row);
            constr[row][zbase + (i - 1) * m + j] = 1;
            constr[row][xbase +  j] = -1;
            constr[row][vbase + (i - 1) * m + j] = -M;
            glp_set_row_name(lp, row, name);
            glp_set_row_bnds(lp, row, GLP_LO, -M * 1.0, 0.0);
            row++;
        }


        //Sum (yij - zij) -Di >= 0
        for(int j = 0; j < m; j++){
            char name[LEN];
            sprintf(name, "row%d", row);
            constr[row][ybase + (i - 1) * m + j] = 1;
            constr[row][zbase + (i - 1) * m + j] = -1;
            glp_set_row_name(lp, row, name);
            glp_set_row_bnds(lp, row, GLP_LO, 1.0 * D[i], 0.0);
        }
        row++;

    }






    /* fill columns bound, name, kind and coef*/
    glp_add_cols(lp, cols);

    for(int k = 0; k < m * m; k++){
        char uname[LEN];
        sprintf(uname, "u%d", k);
        glp_set_col_name(lp, ubase + k, uname);
        glp_set_col_bnds(lp, ubase + k, GLP_DB, 0.0, 1.0);
        glp_set_obj_coef(lp, ubase + k, 0.0);
        glp_set_col_kind(lp, ubase + k, GLP_IV);

        char vname[LEN];
        sprintf(vname, "v%d", k);
        glp_set_col_name(lp, vbase + k, vname);
        glp_set_col_bnds(lp, vbase + k, GLP_DB, 0.0, 1.0);
        glp_set_obj_coef(lp, vbase + k, 0.0);
        glp_set_col_kind(lp, vbase + k, GLP_IV);


        char yname[LEN];
        sprintf(yname, "y%d", k);
        glp_set_col_name(lp, ybase + k, yname);
        glp_set_col_bnds(lp, ybase + k, GLP_LO, 0.0, 0.0);
        glp_set_obj_coef(lp, ybase + k, 0.0);

        char zname[LEN];
        sprintf(zname, "v%d", k);
        glp_set_col_name(lp, zbase + k, zname);
        glp_set_col_bnds(lp, zbase + k, GLP_LO, 0.0, 0.0);
        glp_set_obj_coef(lp, zbase + k, 0.0);
    }

    for(int k = 0; k < m; k++){
        char xname[LEN];
        sprintf(xname, "x%d", k);
        glp_set_col_name(lp, xbase + k, xname);
        glp_set_col_bnds(lp, xbase + k, GLP_LO, 0.0, 0.0);
        glp_set_obj_coef(lp, xbase + k, 1.0);
    }


    // Createing ia, ja and ar array that has to be given to the load matrix function
    int size = (rows) * (cols) + 1;
    int ia[size], ja[size];
    double ar[size];
    int k = 1;
    ia[0] = ja[0] = ar[0] = 0;

    // Copying data from constraint 2D array to ar 1D array and setting the index arrays ia and ja
    for(int i = 1; i <= rows; i++){
        for(int j = 1; j <= cols; j++){
            ia[k] = i;
            ja[k] = j;
            ar[k] = constr[i][j];
            
            k++;
        }
        
    }

    printf("\n");

    glp_load_matrix(lp, size - 1, ia, ja, ar);


    /* integer optimizer control parameters */
    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.presolve = GLP_ON;

    /* integer optimizer */
    int err = glp_intopt(lp, &parm);


    
    // Array to stores outputs
    double z, x[m + 1];
    int u[m + 1][m + 1], v[m + 1][m + 1];
    z = glp_mip_obj_val(lp);


    // Getting optimal col values
    for(int i = 1; i <= m; i++){
        for(int j = 1; j <= m; j++){
            u[i][j] = glp_mip_col_val(lp, ubase + (i - 1) * m + (j - 1));
            v[i][j] = glp_mip_col_val(lp, vbase + (i - 1) * m + (j - 1));
        }
    }

    for(int j = 1; j <= m; j++){
        x[j] = glp_mip_col_val(lp, xbase + j - 1);
    }



    /* recover and display results */
    printf("\n===============================================================\n");
    printf("Results\n\n");
    printf("Optimal value of objective function z = %g\n\n", z);

    printf("%-6s", "xj's");
    for(int j = 1; j <= m; j++)
        if(x[j] != 0)
            printf("%4.1lf ", x[j]);
    printf("\n\n");

    for(int i = 1; i <= m; i++){
        printf("u%dj's ", i);
        for(int j = 1; j <= m; j++){
            if(x[j] != 0)
                printf("%4d ", u[i][j]);
        }
        printf("\n");
        printf("v%dj's ", i);

        for(int j = 1; j <= m; j++){
            if(x[j] != 0)
                printf("%4d ", v[i][j]);
        }

        printf("\n\n");
    }


    printf("\n");

    /* housekeeping */
    glp_delete_prob(lp);
    glp_free_env();

}
