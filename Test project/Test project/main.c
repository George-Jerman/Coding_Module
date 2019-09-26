#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MATRIX_SIZE 3

/* Program to do simple unit conversions from a list given */


//matrices structure definitions
typedef struct Matrix
{
	float elementA[MATRIX_SIZE][MATRIX_SIZE];
	float elementB[MATRIX_SIZE][MATRIX_SIZE];
	float elementC[MATRIX_SIZE][MATRIX_SIZE];
} Matrix;


//function declarations

int conversion(void);
int matrix_multiplier(Matrix *p);
int Temp_fc(void);
int Temp_cf(void);
int Mass_pk(void);
int Mass_kp(void);
void fileA(Matrix *p);
void fileB(Matrix *p);


int main()
{
	
	
	Matrix *p = NULL;
	p = malloc(sizeof *p);
	if (p == NULL)
	{
		fprintf(stderr, "Out of memory!\n");
		exit(99);
	}
	int s = 1;
	printf("1. Matrix multiplier or 2. converter? \n");
	scanf("%d", &s);
	switch (s) {
		case 1:
			matrix_multiplier(p);
			break;
		case 2:
			conversion();
			break;
		default:
			break;
	}
	return 0;
}


//functions

int conversion()
{

int carryon = 1;
int input_choice;
//switch to select category of conversion with an exit/continuation condition

while (carryon == 1) {
	printf("What would you like to convert? \n");
	printf(" 1. Farenheit to Celsius \n 2. Celsius to Farenheit \n 3. Pounds to Kilograms \n 4. Kilograms to Pounds \n 0. Exit \n");
	scanf("%d", &input_choice );
	

	switch (input_choice) {
		case 1:
			Temp_fc();
			break;
		case 2:
			Temp_cf();
			break;
		case 3:
			Mass_pk();
			break;
		case 4:
			Mass_kp();
			break;
		case 0:
			printf("You have exited the program \n");
			return 0;
		default:
			return 1;
			break;
	}
	printf("would you like to convert another value? Press 1 if so or press 0 to quit. \n");
	scanf("%d", &carryon);
}
	return 0;
}


//farenheit to celsius
int Temp_fc()
{
    float tbc;
    float converted_val;
    printf("Please input the value you wish to convert \n");
    scanf("%f", &tbc);
    converted_val = ((tbc - 32) * 5) /9;
    printf("Your value is %f \n", converted_val);
    return 0;
    
}
//celsius to farenheit
int Temp_cf()
{
    float tbc;
    float converted_val;
    printf("Please input the value you wish to convert \n");
    scanf("%f", &tbc);
    converted_val = ((tbc * 9) /5) + 32;
    printf("Your value is %f \n", converted_val);
    return 0;
    
}
//pounds to kilos
int Mass_pk()
{
    float tbc;
    float converted_val;
    printf("Please input the value you wish to convert \n");
    scanf("%f", &tbc);
    converted_val = tbc / 2.2;
    printf("Your value is %f \n", converted_val);
    return 0;
    
}
//kilos to pounds
int Mass_kp()
{
    float tbc;
    float converted_val;
    printf("Please input the value you wish to convert \n");
    scanf("%f", &tbc);
    converted_val = tbc * 2.2;
    printf("Your value is %f \n", converted_val);
    return 0;
    
}

//reads in file matrixA.txt for matrixA
void fileA(Matrix *p)
{
	int x,y;
	FILE *matrixA;
	matrixA = fopen("matrixA.txt", "r");
	
	if (matrixA == NULL) {
		perror("fopen");
		exit(1);
	}
	for (x =0; x < 3; x++) {
		for(y = 0; y < 3; y++)
		{
			fscanf(matrixA, "%f", &p->elementA[x][y]);
		}
	}
	fclose(matrixA);
}

//reads in file matrixB.txt for matrixB2
void fileB(Matrix *p)
{
	int x,y;
	FILE *matrixB;
	matrixB = fopen("matrixB.txt", "r");
	
	if (matrixB == NULL) {
		perror("fopen");
		exit(1);
	}
	for (x =0; x < 3; x++) {
		for(y = 0; y < 3; y++)
		{
			fscanf(matrixB, "%f", &p->elementB[x][y]);
		}
	}fclose(matrixB);
}
//matrix multiplication funciton making use of the matrix structure defined above
int matrix_multiplier(Matrix *p)
{
	int i,j,k;
	int sum = 0;
	//multiplying the matrices
	
	fileA(p);
	fileB(p);
	
	for (i=0; i<MATRIX_SIZE; i++) {
		for (j=0; j<MATRIX_SIZE; j++) {
			for (k=0; k<MATRIX_SIZE; k++) {
				sum = sum + p->elementA[i][k]*p->elementB[k][j];
			}
			p->elementC[i][j] = sum;
			sum =0;
		}
	}
	
	for (i=0; i<MATRIX_SIZE; i++) {
		for (j=0; j<MATRIX_SIZE; j++) {
			printf("%.2f \t", p->elementC[i][j]);
		}
		printf("\n");
	}
	return 0;
}


