//
//  George_Jerman_Assignment1.c
//  Assignment_1
//
//  Version 1.0.3 - 30/10/19
//  Tidied up the code eg removing commented out print statements for testing and added a manual comment below.
//
//  Version 1.0.2 -29/10/19
//  Added in some basic error checks for the file handling eg too many or too few columns, text in input etc
//
//  Version 1.0.1 - 29/10/19
//  Now the program uses sgets alongside sscanf to read in inputs instead of fscanf.
//
//  Version 1.0 - 29/10/19
//  All assigned tasks work as sepcified, however still using fscanf to read in files and only the simplest test cases for errors.
//
//
//  Created by George Jerman on 14/10/2019.
//  General command line argument code taken from https://vle.exeter.ac.uk/mod/resource/view.php?id=961497
//  Author: C.D.H.Williams and adapted by George Jerman for use in this project under Public Domain
//
//  Code snippet for measuring the time a function takes to run taken from
//  https://stackoverflow.com/questions/10192903/time-in-milliseconds-in-c
//
//
/*
 ---------------------------------------------------------------------MANUAL-----------------------------------------------------------------------------
 This program takes in 1 or 2 files of the following format:
 
 # ./mat_gen --rows 3 --cols 3
 # Version = 1.0.2, Revision date = 15-Oct-2019
 matrix 3 3
 0.150241317297    0.0777884503257    0.647563110407
 0.978028524191    0.139817787865    0.446850332174
 0.429817350781    0.710610827296    0.349869806017
 end
 
 in which lines beginning with a hash are comments and are ignored by the program. The first line not beginning with # must be of the format 'matrix m n'
 where m and n are the number of rows and columns contained in the matrix in the file. Then the data is written in tab separated format and the line
 after the final data entry should contain the word 'end'. The output format of this program is the same as the input format. allowing it to be used
 in a scripted workflow. The way the command line arguments should be structued is as follows.
 
 ./mat_test --file1 input1.txt -f

 or if performing an operation that requires 2 matrices (in this case only multiplication) the format is:
 
 ./mat_test --file1 input1.txt --file2 input2.txt -m
 
 with input1.txt anf input2.txt representing the files the user wishes to use. The list of operations supported and their flags is as follows:
 
 Frobenius norm   -f
 Transpose        -t
 Multiplication   -m
 Determinant      -d
 Adjoint          -a
 Inverse          -i
 
 The program only supports use of one operation at a time, so if multiple are required, the program will need to be ran separately in each case.
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <string.h>
#include <math.h>
//#include <sys/time.h>

#define MAX_LINE_LENGTH 1000000

//struct timespec start, end;
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

//Function prototypes
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
    char version_num[] = "Version 1.0.3 Date modified - 30/10/19";
    
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
                //print out the command used to create the file at the top of the output.
                printf( "# ");
                for ( int arg_no = 0; arg_no  <argc; arg_no++ ){
                    printf("%s ", argv[arg_no]);
                }
                printf("\n");
                printf("# %s", version_num);

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
                matrix_product(mat1, mat2, rows, rows2, columns, columns2);;
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
                    printf("\n");
                    printf("matrix 1 1\n");
                    printf("%lg\n",answer);
                    printf("end\n");
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
                    free(adjoint_matrix);
                    return NO_ERROR;
                }
                break;
            case 'i':
               // clock_gettime(CLOCK_MONOTONIC_RAW, &start);
                 inverse(mat1, rows);
                //clock_gettime(CLOCK_MONOTONIC_RAW, &end);
               // uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
               // printf("%llu", delta_us);
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

//Function to get the size of the first input matrix by utilsing the "matrix rows columns" line in the generator program provided
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
//allocates enough heap memory for the first array
static double* CreateArray1(int rows, int columns)
{
    double *mat1 = malloc(sizeof(double)*rows*columns);
    return mat1;
}
//allocates enough heap memory for the second array
static double* CreateArray2(int rows2, int columns2)
{
    double *mat2 = malloc(sizeof(double)*rows2*columns2);
    return mat2;
}
//reads in values from the first specified file into the first matrix
static double* Populate_mat1(double* mat1, FILE *matrix_1,int rows,int columns )
{
    int i,j,f,check;
    
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
            if ((f != 2))
            {
                printf("line specifying the number of rows and columns is incorrectly formatted ");
                exit(BAD_ARGS);
                break;
            }
            else
            {
                break;
            }
        }
    }
    if (rows == 0 || columns == 0) {
        printf("Columns or rows specifies 0 elements");
        exit(BAD_FORMAT);
    }
    int temp = 0, count = 0;
    for (i=0; i<rows; i++) {
        fgets(string_buffer, MAX_LINE_LENGTH, matrix_1);
        for (j=0; j<columns; j++) {
            if((check =sscanf(&string_buffer[count], "%lg %n", &mat1[i*columns+j], &temp)) !=1){
                printf("Number of elements in array does not match the number specified\n");
                exit(BAD_FORMAT);
            }
            count += temp;
        }
        //checking to see if there are any unread data in the line
        char error_check;
        if (sscanf(&string_buffer[count], "%c", &error_check) == 1) {
            printf("Number of data exceeds number of columns\n");
            exit(BAD_FORMAT);
        }
        count = temp =0;
    }
    fgets(string_buffer, MAX_LINE_LENGTH, matrix_1);
    if ((f = string_buffer[0] != 'e')) {
        printf("Format must have \"end\" on a newline after the data\n");
        exit(BAD_FORMAT);
    }
    fclose(matrix_1);
    return mat1;
}
//reads in values from the second specified file into the second matrix
static double* Populate_mat2(double* mat2, FILE *matrix_2,int rows2,int columns2)
{
    int i,j,f,check;
    
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
            if ((f != 2))
            {
                printf("line specifying the number of rows and columns is incorrectly formatted ");
                exit(BAD_ARGS);
                break;
            }
            else
            {
                break;
            }
        }
    }
    int temp = 0, count = 0;
    for (i=0; i<rows2; i++) {
        fgets(string_buffer, MAX_LINE_LENGTH, matrix_2);
        for (j=0; j<columns2; j++) {
            sscanf(&string_buffer[count], "%lg %n", &mat2[i*columns2+j], &temp);
            if((check =sscanf(&string_buffer[count], "%lg %n", &mat2[i*columns2+j], &temp)) !=1){
                printf("Number of elements in array does not match the number specified\n");
                exit(BAD_FORMAT);
            }
            count += temp;
        }
        count = temp =0;
    }
    fgets(string_buffer, MAX_LINE_LENGTH, matrix_2);
       if ((f = string_buffer[0] != 'e')) {
           printf("Format must have \"end\" on a newline after the data\n");
           exit(BAD_FORMAT);
       }
    fclose(matrix_2);
    return mat2;
}
//general purpose printing function for matrices
static Error print_matrix(double *mat , int r, int c)
{
    int i,j;
    printf("matrix %d %d\n", r, c);
    for (i=0; i<r; i++) {
        for (j=0; j<c; j++) {
            printf("%.15lg\t", mat[i*c+j]);
        }
        printf("\n");
    }
    printf("end\n");
    return NO_ERROR;
}
//finds the frobenius norm of a specified matrix (in this case the first matrix)
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
    printf("\n");
    printf("matrix 1 1\n");
    printf("%.13lg\n", sum);
    printf("end\n");
    free(mat1);
    return NO_ERROR;
}
//finds the transpose of the specified matrix and returns it for either printing or use in another function eg adjoint.
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
//calculates the matrix product of the 2 inputted matrices
static Error matrix_product(double *mat1, double* mat2, int rows, int columns, int rows2, int columns2)
{
    //chcking if multiplication is possible
    if (columns != rows2) {
        exit(INCOMPATIBLE_MATRICES);
    }
    double *mat_multiplied = malloc(sizeof(double)*columns2*rows);
    int i,j,k;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < columns2; j++) {
            //performs the dot product to find each matrix elememt
            mat_multiplied[i*columns2+j] = 0; /* fixes a bug where occasionally the first 2 elements of the array were returned as very large positive or negative numbers.*/
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
    return 0;
}
//calcualtes the determinant of the specified matrix and returns the value for printing or use in another function eg the inverse.
static double determinant(double * mat1, int size)
{
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
        double * minor_matrix = malloc(sizeof(double)*(size-1)*(size-1));
        for(top_row=0; top_row<size; top_row++){//top row of matrix
            for (i=0; i<size-1; i++) {//rows of cofactor matrix
                ignore_col=0; //current column to be skipped
                for (j=0; j<size; j++) {//columns of cofactor matrix
                    if (j ==top_row) continue; //determines whether to skip a column or not
                    minor_matrix[(i)*(size-1)+ignore_col] = mat1[(i+1)*size+j]; //size-1 due to the minor matrix being 1 smaller than the matrix passed to the function.
                    ignore_col++;
                }
            }
            det += determinant(minor_matrix, size-1) * mat1[0*size+top_row] * pow(-1, top_row);
        }
        free(minor_matrix);
    }
    return det;
}
//uses the transpose matrix generated alongside the determinant function and finds the adjoint of a specified matrix
static double* adjoint(double * input_matrix, int size)
{
    //special case checking
    if (size ==1) {
        double * adj = malloc(sizeof(double)*size*size);
        adj[0] = 1;
        return adj;
    }
    int rows,cols,sign=1;
    int ignore_col,ignore_row;
    int i,j;
    double * matrix_of_minors = malloc(sizeof(double)*(size-1)*(size-1));
    double * cofactor_matrix = malloc(sizeof(double)*size*size);
    //main matrix iteration
    for(rows=0; rows<size; rows++){
        for (cols=0; cols<size; cols++) {
            ignore_row=0;
            ignore_col=0;
            //minor matrix iteration
            for (i=0; i<size; i++){
                if (i == rows) continue;//current column to be skipped
                for (j=0; j<size; j++) {//columns of cofactor matrix
                    if (i != rows && j != cols){
                        matrix_of_minors[ignore_row*(size-1)+ignore_col] = input_matrix[i*size+j];
                        if (ignore_col<(size-2)) { /*signals the end of the row of the minor matrix*/
                            ignore_col++;
                        }
                        else{
                            ignore_col=0;
                            ignore_row++;
                        }
                    }
                }
            }
           //calculates the element of the cofactor matrix from the determinant of the found minor matrix
           cofactor_matrix[rows*size+cols] = determinant(matrix_of_minors, size-1) *sign;
            sign *=-1;
        }
    }
    cofactor_matrix = transpose(cofactor_matrix, size, size);
    //free(matrix_of_minors);
    return cofactor_matrix;
}
//takes both the adjoint and the determinant from the previous functions and uses them to calculate the inverse matrix and prints it.
static Error inverse(double * input_matrix, int size)
{
    if (size ==1){
        printf("\n");
        printf("matrix %d %d\n", size, size);
        printf("%.15lg\n",(1/input_matrix[0]));
        printf("end\n");
        return NO_ERROR;
    }
    
    double * inverse_matrix = malloc(sizeof(double)*size*size);
    int i,j;
    double det = determinant(input_matrix, size);
    double * adjoint_matrix = adjoint(input_matrix, size);
    for (i=0; i<size; i++) {
        for (j=0; j<size; j++) {
            inverse_matrix[i*size+j] = (1/det) * adjoint_matrix[i*size+j];
        }
    }
    print_matrix(inverse_matrix, size, size);
    free(input_matrix);
    free(adjoint_matrix);
    free(inverse_matrix);
    return NO_ERROR;
}
