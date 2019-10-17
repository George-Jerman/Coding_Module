//
//  main.c
//  Assignment_1
//
//  Created by George Jerman on 14/10/2019.
//  Copyright © 2019 George Jerman. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <string.h>

#define MAX_LINE_LENGTH 10000
#define MAX_MATRICES 3


/* Constants for signalling errors: */

typedef enum {
    NO_ERROR = 0,
    NO_MEMORY = 1,
    BAD_ARGS = 2,
    BAD_FILENAME = 3,
    NO_MATRIX_2 = 4,
    BAD_FORMAT = 5,
    UNKNOWN_ERROR = -1
} Error;


static void matrix1_size(FILE * matrix_1, int *rows, int *columns);
static void matrix2_size(FILE * matrix_2, int *rows2, int *columns2);
double ** CreateArray1(int rows, int columns);
double** CreateArray2(int rows2, int columns2);
double** Populate_mat1(double** mat1_placeholder, FILE *matrix_1,int rows,int columns);
double** Populate_mat2(double** mat2_placeholder, FILE *matrix_2,int rows2,int columns2);
static Error frobenius_norm(void);
static Error transpose(void);
static Error matrix_product(void);
static Error determinant(void);
static Error adjoint(void);
static Error inverse(void);


int main(int argc, char ** argv)
{
    int ret_val =0;
    char * input_file_name_1 = NULL;
    char * input_file_name_2 = NULL;
    
    int rows, columns, rows2, columns2;
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
        
    int c = getopt_long( argc, argv, ":z:x:f:t:m:d:a:i:", long_options, &option_index );
    /* End of options is signalled with '-1' */
    while (c != -1) {
        switch (c) {
            case 'z': //when file1 is specified creates an array from the data in input file specified
                input_file_name_1 = optarg;
                matrix_1 =fopen(input_file_name_1, "r");
                if (matrix_1 == NULL) {
                    return BAD_FILENAME;
                }
                matrix1_size(matrix_1, &rows, &columns);
                double **mat1_placeholder =CreateArray1(rows, columns);
                printf("Matrix1\n");
                double **mat1 =Populate_mat1(mat1_placeholder, matrix_1, rows, columns);
                break;
            case 'x': //when file2 is specified creates an array from the data in input file specified
                input_file_name_2 = optarg;
                matrix_2 =fopen(input_file_name_2, "r");
                if (matrix_2 == NULL) {
                    return BAD_FILENAME;
                }
                matrix2_size(matrix_2, &rows2, &columns2);
                double **mat2_placeholder =CreateArray2(rows2, columns2);
                printf("Matrix2\n");
                double **mat2 =Populate_mat2(mat2_placeholder, matrix_2, rows2, columns2);
                break;
            case 'f':
                ret_val = frobenius_norm();
                break;
            case 't':
                ret_val = transpose();
                break;
            case 'm':
                ret_val = matrix_product();
                break;
            case 'd':
                ret_val = determinant();
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
        c = getopt_long( argc, argv, ":z:x:f:t:m:d:a:i:", long_options, &option_index );
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

double** CreateArray1(int rows, int columns)
{
    int i;
    double**mat1 = (double**) malloc(sizeof(int*)*rows);
    for (i=0; i<rows; i++) {
        mat1[i] = (double*) malloc(sizeof(int*)*columns);
    }
    return mat1;
}

double** CreateArray2(int rows2, int columns2)
{
    int i;
    double**mat2_placeholder = (double**) malloc(sizeof(int*)*rows2);
    for (i=0; i<rows2; i++) {
        mat2_placeholder[i] = (double*) malloc(sizeof(int*)*columns2);
    }
    return mat2_placeholder;
}

double** Populate_mat1(double** mat1_placeholder, FILE *matrix_1,int rows,int columns )
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
    }
    for (i=0; i<rows; i++) {
        for (j=0; j<columns; j++) {
            f = fscanf(matrix_1, "%lf", &mat1_placeholder[i][j]);
            if (f != 1) {
                exit(BAD_FORMAT);
            }
            printf("%lf\t", mat1_placeholder[i][j]);
        }
        printf("\n");
    }
    fclose(matrix_1);
    return mat1_placeholder;
}

double** Populate_mat2(double** mat2_placeholder, FILE *matrix_2,int rows2,int columns2)
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
            f = fscanf(matrix_2, "%lf", &mat2_placeholder[i][j]);
            if (f != 1) {
                exit(BAD_FORMAT);
            }
            printf("%lf\t", mat2_placeholder[i][j]);
        }
        printf("\n");
    }
    fclose(matrix_2);
    return mat2_placeholder;
}

static Error frobenius_norm(void)
{
    return NO_ERROR;
}

static Error transpose(void)
{
    return NO_ERROR;
}

static Error matrix_product(void)
{
    return NO_ERROR;
}

static Error determinant(void)
{
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
