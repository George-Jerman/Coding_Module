//
//  main.c
//  Assignment_1
//  Version 0.4
//  Changed arrays to a more efficient method
//  Created by George Jerman on 14/10/2019.
//  General command line argument code taken from mat_gen.c from C.D.H.Williams and adapted.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#define MAX_LINE_LENGTH 10000


/* Constants for signalling errors: */

typedef enum {
    NO_ERROR = 0,
    NO_MEMORY = 1,
    BAD_ARGS = 2,
    BAD_FILENAME = 3,
    NO_MATRIX_2 = 4,
    BAD_FORMAT = 5,
    INCOMPATIBLE_MATRICES =6,
    UNKNOWN_ERROR = -1
} Error;

struct timespec start, end;

static void matrix1_size(FILE * matrix_1, int *rows, int *columns);
static void matrix2_size(FILE * matrix_2, int *rows2, int *columns2);
static double* CreateArray1(int rows, int columns);
static double* CreateArray2(int rows2, int columns2);
static double* Populate_mat1(double* mat1, FILE *matrix_1,int rows,int columns);
static double* Populate_mat2(double* mat2, FILE *matrix_2,int rows2,int columns2);
static Error print_matrix(double *mat , int r, int c);
static Error frobenius_norm(double* mat1, int rows, int columns);
static double* transpose(double* mat1, int columns, int rows);
static Error matrix_product(double *mat1, double* mat2, int rows, int columns, int rows2, int columns2);
static double determinant(double * mat1, int size);
static double* adjoint(double * input_matrix, int size);
static Error inverse(double * input_matrix, int size);

int main(int argc, char ** argv)
{
    int ret_val =0;
    char * input_file_name_1 = NULL;
    char * input_file_name_2 = NULL;
    
    int rows = 0, columns = 0, rows2 = 0, columns2= 0, size =0;
    FILE *matrix_1;
    FILE *matrix_2;
    
    static struct option long_options[] = {
        /* These options don’t set a flag the are edistinguished by their indices. */
        {"file1",  required_argument,  0, 'z'},
        {"file2",  required_argument,  0, 'x'},
        {"f",      required_argument,  0, 'f'},
        {"t",      required_argument,  0, 't'},
        {"m",      required_argument,  0, 'm'},
        {"d",      required_argument,  0, 'd'},
        {"a",      required_argument,  0, 'a'},
        {"i",      required_argument,  0, 'i'},
        {0, 0, 0, 0}
        };
        
    /* getopt_long needs somewhere to store its option index. */
    int option_index = 0;
        
    int c = getopt_long( argc, argv, ":z:x:ftmdai", long_options, &option_index );
    /* End of options is signalled with '-1' */
    while (c != -1) {
        switch (c) {
            case 'z': //when file1 is specified creates an array from the data in input file specified
                input_file_name_1 = optarg;
                matrix_1 =fopen(input_file_name_1, "r");
                if (matrix_1 == NULL) {
                    printf("This file does not exist.\n");
                    return BAD_FILENAME;
                }
                matrix1_size(matrix_1, &rows, &columns);
                double *mat1=CreateArray1(rows, columns);
                mat1 =Populate_mat1(mat1, matrix_1, rows, columns);

                break;
            case 'x': //when file2 is specified creates an array from the data in input file specified
                input_file_name_2 = optarg;
                matrix_2 =fopen(input_file_name_2, "r");
                if (matrix_2 == NULL) {
                    printf("This file does not exist.\n");
                    return BAD_FILENAME;
                }
                matrix2_size(matrix_2, &rows2, &columns2);
                double *mat2 =CreateArray2(rows2, columns2);
                mat2 =Populate_mat2(mat2, matrix_2, rows2, columns2);
                break;
            case 'f':
                if (rows ==0 || columns ==0) {
                    printf("You need to specify the input file before attempting to pick an operation to do to it.\n");
                    return BAD_FORMAT;
                }
                return frobenius_norm(mat1, rows, columns);

                break;
            case 't':
                if (mat1) {
                    double *mat_transpose = transpose(mat1, rows, columns);
                    print_matrix(mat_transpose, rows, columns);
                    free(mat_transpose);
                    return NO_ERROR;
                }
                else{
                    return BAD_FILENAME;
                }

                break;
            case 'm':
                matrix_product(mat1, mat2, rows, rows2, columns, columns2);
                
                return NO_ERROR;
                break;
            case 'd':
                if (rows != columns) {
                    return BAD_FORMAT;
                }
                else
                {
                    size = rows;
                    double answer =determinant(mat1, size);
                    printf("%lg\n",answer);
                }
                
                break;
            case 'a':
                if (rows != columns) {
                    return BAD_FORMAT;
                }
                else
                {
                    size = rows;
                    double * adjoint_matrix = malloc(sizeof(double)*size*size);
                    adjoint_matrix = adjoint(mat1, size);
                    print_matrix(adjoint_matrix, size, size);
                    return 0;
                }
                
                break;
            case 'i':
                ret_val = inverse(mat1, rows);
                break;
            case ':':
                /* missing option argument */
                fprintf(stderr, "Error: option '-%c' requires an argument\n", optopt);
                ret_val = BAD_ARGS;
                break;
            case '?':
            default:
                /* invalid option */
                fprintf(stderr, "Warning: option '-%c' is invalid: ignored\n", optopt);
                break;
        }
        c = getopt_long( argc, argv, ":z:x:ftmdai", long_options, &option_index );
    }
    return NO_ERROR;
}

//Function to get the size of the firsr input matrix by utilsing the "matrix rows columns" line in the generator program provided
static void matrix1_size(FILE * matrix_1,int *rows, int *columns)
{
    int f;
    char string_buffer[MAX_LINE_LENGTH];
    
    while((fgets(string_buffer, MAX_LINE_LENGTH, matrix_1)) !=NULL)
    {
        if (string_buffer[0] == '#')
        {
            ;
        }
        else if(strstr(string_buffer, "matrix"))
        {
            f = sscanf(string_buffer,"%*s %d %d", rows, columns);
            if ((f = 2))
            {
                ;
            }
            else
            {
                exit(BAD_ARGS);
            }
        }
        else
        {
            break;
        }
    }
    fseek(matrix_1, 0, SEEK_SET);
}
//Function to get the size of the second input matrix as in the above function
static void matrix2_size(FILE * matrix_2, int *rows2, int *columns2)
{
    int f;
    
    char string_buffer[MAX_LINE_LENGTH];
    while((fgets(string_buffer, MAX_LINE_LENGTH, matrix_2)) !=NULL)
    {
        if (string_buffer[0] == '#')
        {
            ;
        }
        else if(strstr(string_buffer, "matrix"))
        {
            f = sscanf(string_buffer,"%*s %d %d", rows2, columns2);
            if ((f = 2))
            {
                ;
            }
            else
            {
                exit(BAD_ARGS);
            }
        }
        else
        {
            break;
        }
    }
    fseek(matrix_2, 0, SEEK_SET);
}

static double* CreateArray1(int rows, int columns)
{
    double *mat1 = malloc(sizeof(double)*rows*columns);
    return mat1;
}

static double* CreateArray2(int rows2, int columns2)
{
    double *mat2 = malloc(sizeof(double)*rows2*columns2);
    return mat2;
}

static double* Populate_mat1(double* mat1, FILE *matrix_1,int rows,int columns )
{
    int i,j,f;
    
    char string_buffer[MAX_LINE_LENGTH];
    while((fgets(string_buffer, MAX_LINE_LENGTH, matrix_1)) !=NULL)
    {
        if (string_buffer[0] == '#')
        {
            ;
        }
        else if(strstr(string_buffer, "matrix"))
        {
            f = sscanf(string_buffer,"%*s %d %d", &rows, &columns);
            if ((f = 2))
            {
                break;
            }
            else
            {
                exit(BAD_ARGS);
            }
        }
        /*else{
            for (i=0; i<rows; i++) {
                for (j=0; j<columns; j++) {
                    f = sscanf(string_buffer, "%lg", &mat1[i*columns+j]);
                    if (f != 1) {
                        exit(BAD_FORMAT);
                    }
                   // printf("%.13lg\t", mat1[i*columns+j]);
                }
            //printf("\n");
            }
        }*/
    }
    for (i=0; i<rows; i++) {
        for (j=0; j<columns; j++) {
            f = fscanf(matrix_1, "%lg", &mat1[i*columns+j]);
            if (f != 1) {
                exit(BAD_FORMAT);
            }
           // printf("%.13lg\t", mat1[i*columns+j]);
        }
    //printf("\n");
    }
    fclose(matrix_1);
    return mat1;
}

static double* Populate_mat2(double* mat2, FILE *matrix_2,int rows2,int columns2)
{
    int i,j,f;
    
    char string_buffer[MAX_LINE_LENGTH];
    while((fgets(string_buffer, MAX_LINE_LENGTH, matrix_2)) !=NULL)
    {
        if (string_buffer[0] == '#')
        {
            ;
        }
        else if(strstr(string_buffer, "matrix"))
        {
            f = sscanf(string_buffer,"%*s %d %d", &rows2, &columns2);
            if ((f = 2))
            {
                break;
            }
            else
            {
                exit(BAD_ARGS);
            }
        }
    }
    for (i=0; i<rows2; i++) {
        for (j=0; j<columns2; j++) {
            f = fscanf(matrix_2, "%lg", &mat2[i*columns2+j]);
            if (f != 1) {
                exit(BAD_FORMAT);
            }
            //printf("%.13g\t", mat2[i*columns2+j]);
        }
        //printf("\n");
    }
    fclose(matrix_2);
    return mat2;
}

static Error print_matrix(double *mat , int r, int c)
{
    int i,j;
    printf("matrix %d %d\n", r, c);
    for (i=0; i<r; i++) {
        for (j=0; j<c; j++) {
            printf("%.18lg\t", mat[i*c+j]);
        }
        printf("\n");
    }
    return NO_ERROR;
}

static Error frobenius_norm(double * mat1, int rows, int columns)
{
    clock_gettime(_CLOCK_MONOTONIC_RAW, &start);
    int i,j;
    double element=0,sum=0;
    for (i=0; i<rows; i++) {
        for (j=0; j<columns; j++) {
            element = pow(mat1[i*columns+j], 2);
            sum += element;
        }
    }
    sum = sqrt(sum);
    printf("The frobenius norm of the matrix is: %.13lg\n", sum);
    free(mat1);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    double time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec -start.tv_nsec) / 1000;
    time = time/100000;
    printf("Time taken: %.6lg seconds\n", time);
    return NO_ERROR;
}

static double* transpose(double *mat1, int rows, int columns)
{
    printf("\n");
    int i,j;
    //define transpose array
    double *mat_Transpose = malloc(sizeof(double)*columns*rows);
    //columns and rows are swapped in transpose
    for (i=0; i<rows; i++) {
        for (j=0; j<columns; j++) {
            mat_Transpose[(j*rows)+i] = mat1[(i*columns)+j];
        }
    }
    free(mat1);
    return mat_Transpose;
}

static Error matrix_product(double *mat1, double* mat2, int rows, int columns, int rows2, int columns2)
{
    FILE * destination =fopen("output.txt", "w");
    //chcking if multiplication is possible
    if (columns != rows2) {
        exit(INCOMPATIBLE_MATRICES);
    }
    double *mat_multiplied = malloc(sizeof(double)*columns2*rows);
    int i,j,k;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < columns2; j++) {
            for (k = 0; k < columns; k++) {
                mat_multiplied[(i*columns2)+j] += mat1[(i*columns)+k] * mat2[(k*columns2)+j];
            }
        }
    }
    free(mat1);
    free(mat2);
    printf("\n");
    print_matrix(mat_multiplied, rows, columns2);
    free(mat_multiplied);

    fclose(destination);
    return 0;
}

static double determinant(double * mat1, int size)
{
    //print_matrix(mat1, size, size);
    double det=0;
    if (size == 1) {
        det = mat1[0];
        return det;
    }
    else if (size == 2)
    {
        det = (mat1[0*size+0] * mat1[(1*size) +1]) - (mat1[1*size +0] * mat1[0*size+1]);
        return det;
    }
    else
    {
        int top_row;
        int i,j;
        int ignore_col;
        det = 0;
        double * cofactor_matrix = malloc(sizeof(double)*(size-1)*(size-1));
        for(top_row=0; top_row<size; top_row++){//top row of matrix
            for (i=0; i<size-1; i++) {//rows of cofactor matrix
                ignore_col=0; //current column to be skipped
                for (j=0; j<size; j++) {//columns of cofactor matrix
                    if (j ==top_row) continue; //determines whether to skip a column or not
                    cofactor_matrix[(i)*(size-1)+ignore_col] = mat1[(i+1)*size+j];
                    ignore_col++;
                }
            }
            //print_matrix(cofactor_matrix, size-1, size-1);
            //printf("\n");
            det += determinant(cofactor_matrix, size-1) * mat1[0*size+top_row] * pow(-1, top_row);
        }
        free(cofactor_matrix);
    }
    return det;
}
static double* adjoint(double * input_matrix, int size)
{
    int rows,cols,sign=1;
    int ignore_col,ignore_row;
    int i,j;
    //double det =0;
    double * matrix_of_minors = malloc(sizeof(double)*size*size);
    double * cofactor_matrix = malloc(sizeof(double)*(size-1)*(size-1));
    for(rows=0; rows<size; rows++){ //for rows and cols in main matrix
        //ignore_row=0;
        for (cols=0; cols<size; cols++) {
            ignore_row=0;
            ignore_col=0;
            for (i=0; i<size; i++){
                if (i == rows) continue;//current column to be skipped
                for (j=0; j<size; j++) {//columns of cofactor matrix
                    if (i != rows && j != cols){
                        cofactor_matrix[ignore_row*(size-1)+ignore_col] = input_matrix[i*size+j];
                        if (ignore_col<(size-2)) {
                            ignore_col++;
                        }
                        else{
                            ignore_col=0;
                            ignore_row++;
                        }
                    }
                }
            }
            //print_matrix(cofactor_matrix, size-1, size-1);
            //printf("\n");
            matrix_of_minors[rows*size+cols] = determinant(cofactor_matrix, size-1) *sign;
            sign *=-1;
        }
    }
    //printf("\n");
    //print_matrix(matrix_of_minors, size, size);
    matrix_of_minors = transpose(matrix_of_minors, size, size);
    return matrix_of_minors;
}

static Error inverse(double * input_matrix, int size)
{
    double * inverse_matrix = malloc(sizeof(double)*size*size);
    int i,j;
    double det = determinant(input_matrix, size);
    double * adjoint_matrix = adjoint(input_matrix, size);
    //print_matrix(adjoint_matrix, size, size);
    for (i=0; i<size; i++) {
        for (j=0; j<size; j++) {
            inverse_matrix[i*size+j] = (1/det) * adjoint_matrix[i*size+j];
        }
    }
    print_matrix(inverse_matrix, size, size);
    return NO_ERROR;
}
