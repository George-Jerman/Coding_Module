#include <stdio.h>
#include <math.h>

int main()
{
	printf("Hello World\n");
    
    float x;
    float y;
    float z;
    printf("What would you like to add together? \n");
    scanf("%f", &y);
    scanf("%f", &z);
    x = y + z;
    printf("The answer is %f \n" , x);
	return 0;
}


