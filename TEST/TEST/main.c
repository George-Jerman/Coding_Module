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


static void matrix1_size(FILE * matrix_1, int *rows, int *columns);
static void matrix2_size(FILE * matrix_2, int *rows2, int *columns2);
double* CreateArray1(int rows, int columns);
double* CreateArray2(int rows2, int columns2);
double* Populate_mat1(double* mat1, FILE *matrix_1,int rows,int columns);
double* Populate_mat2(double* mat2, FILE *matrix_2,int rows2,int columns2);
static Error print_matrix(double *mat , int r, int c);
static Error frobenius_norm(double* mat1, int rows, int columns);
static Error transpose(double* mat1, int columns, int rows);
static Error matrix_product(double *mat1, double* mat2, int rows, int columns, int rows2, int columns2);
static double determinant(double * mat1, int rank);
static Error adjoint(void);
static Error inverse(void);


int main(int argc, char ** argv)
{
    int ret_val =0;
    char * input_file_name_1 = NULL;
    char * input_file_name_2 = NULL;
    
    int rows = 0, columns = 0, rows2 = 0, columns2= 0, rank =0;
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
        
    int c = getopt_long( argc, argv, ":z:x:ftmda:i:", long_options, &option_index );
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
                return transpose(mat1, rows, columns);
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
                    rank = rows;
                    double answer =determinant(mat1, rank);
                    printf("%lg\n",answer);
                }
                
                break;
            case 'a':
                ret_val = adjoint();
                break;
            case 'i':
                ret_val = inverse();
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
        c = getopt_long( argc, argv, ":z:x:ftmda:i:", long_options, &option_index );
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

double* CreateArray1(int rows, int columns)
{
    double *mat1 = malloc(sizeof(double)*rows*columns);
    return mat1;
}

double* CreateArray2(int rows2, int columns2)
{
    double *mat2 = malloc(sizeof(double)*rows2*columns2);
    return mat2;
}

double* Populate_mat1(double* mat1, FILE *matrix_1,int rows,int columns )
{
    int i,j,f;
    int num=0, count=0;
    
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
        else{
            for (i=0; i<rows; i++) {
                for (j=0; j<columns; j++) {
                    f = sscanf(&string_buffer[count], "%lg %n", &mat1[i*columns+j], &num);
                    count += num;
                    if (f != 1) {
                        exit(BAD_FORMAT);
                    }
                    printf("%.13lg\t", mat1[i*columns+j]);
                }
            printf("\n");
            }
        }
    }
    fclose(matrix_1);
    return mat1;
}

double* Populate_mat2(double* mat2, FILE *matrix_2,int rows2,int columns2)
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
    return NO_ERROR;
}

static Error transpose(double *mat1, int rows, int columns)
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
    print_matrix(mat_Transpose, columns, rows);
    free(mat_Transpose);
    return NO_ERROR;
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

static double determinant(double * mat1, int rank)
{
    printf("%d\n", rank);
    //print_matrix(mat1, rank, rank);
    double det=0;
    if (rank == 1) {
        det = mat1[0];
        return det;
    }
    else if (rank == 2)
    {
        printf("Hello");
        det = (mat1[0] * mat1[(1*rank) +1]) - (mat1[(1*rank) +0] * mat1[(0*rank)+1]);
        return det;
    }
    else
    {
        printf("\n");
        int top_row;
        int i,j;
        int ignore_col;
        int sign = 1;
        int a_ij=0;
        det = 0;
        double * cofactor_matrix = malloc(sizeof(double)*(rank-1)*(rank-1));
        for(top_row =0; top_row<rank;top_row++)
        {
            for (i=1; i<rank; i++) {
                ignore_col=0;

                for (j=0; j<rank; j++) {
                    if (top_row ==j) {
                        ignore_col = 1;
                    }
                    cofactor_matrix[(i-1)*(rank-1)+j-ignore_col] = mat1[i*rank+j];
                    printf("%d", ignore_col);
                }
            }
            a_ij++;
            printf("\n");
            print_matrix(cofactor_matrix, rank-1, rank-1);
            printf("\n");
        }
        free(cofactor_matrix);
        det = determinant(mat1, rank-1) * mat1[0*rank+a_ij] * sign;
        sign *= -1;
    }
    return NO_ERROR;
}
static Error adjoint(void)
{
    return NO_ERROR;
}

static Error inverse(void)
{
    return NO_ERROR;
}

