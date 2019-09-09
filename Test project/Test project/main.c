#include <stdio.h>
#include <math.h>

/* Program to do simple unit conversions from a list given */

//function declarations
float Temp_fc(void);
float Temp_cf(void);
float Mass_pk(void);
float Mass_kp(void);


int main()
{
    int input_choice;
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
}
//farenheit to celsius
float Temp_fc()
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
float Temp_cf()
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
float Mass_pk()
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
float Mass_kp()
{
    float tbc;
    float converted_val;
    printf("Please input the value you wish to convert \n");
    scanf("%f", &tbc);
    converted_val = tbc * 2.2;
    printf("Your value is %f \n", converted_val);
    return 0;
    
}
