//
//  main.c
//  Final_Project
//
//  Created by George Jerman on 14/01/2020.
//  Copyright © 2020 George Jerman. All rights reserved.
//

/*---------includes-------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <gsl/gsl_rng.h>
#include <time.h>
#include <assert.h>

/*---------definitions-----------*/
#define BASE_10 10
#define MU_0 1.25663706212e-6 /*units of H/m*/
#define K_B 1.380649e-23 /*unita od J/K*/
#define xfree(mem) { free( mem ); mem = NULL; } //checks if free is successful
#define S(i,j) (spin[((Quantities->array_size+(i))%Quantities->array_size)*Quantities->array_size+((Quantities->array_size+(j))%Quantities->array_size)]) /* Makes referring to the array simpler and also deals
                                    with the periodic BCs required to simulate an infinite lattice
                                    i.e this means that the N * N array element wraps around to the 0th
                                    element*/

typedef enum {
    NO_ERROR = 0,
    NO_MEMORY = 1,
    BAD_ARGS = 2,
    BAD_FILENAME = 3,
    BAD_FORMAT = 4,
    BAD_FILE_OPEN = 5,
    UNKNOWN_ERROR = -1,
    BAD_MALLOC = 100
} Error;

//put the user specifed constants as well as energy in this struct to minimise
//number of variables to pass to the functions
typedef struct Quantities {
    double J;
    double B;
    double temperature;
    long double total_energy;
    double delta_e;
    int array_size;
    long number_of_iterations;
} quantities;


/*-----------prototypes-----------*/
static void * xmalloc(size_t bytes);
static Error get_double_arg( double *value, const char *opt_name, char *optarg);
static int checked_strtoi(const char *string);
static long checked_strtol(const char *string);
static Error populate_array(int *spin, quantities * Quantities, gsl_rng *r);
static double calculate_energy (int *spin, quantities * Quantities);
static double calculate_energy_change (int *spin, quantities * Quantities, unsigned long i, unsigned long j);
static Error metropolis_implementation (int *spin, quantities * Quantities, gsl_rng * rng_struct);

int main(int argc, char ** argv) {

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);


    static struct option long_options[] = {
    /* These options don’t set a flag the are edistinguished by their indices. */
    {"matrix_size", required_argument,  0, 'd'},
    {"J", required_argument, 0, 'j'},
    {"B", required_argument, 0, 'b'},
    {"Temperature", required_argument, 0, 't'},
    {"Iterations", required_argument, 0, 'i'},
    {0, 0, 0, 0}
    };
    //tells the program to use the mersenne twister rng algorithm.
    gsl_rng * random_num_gen_struct = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(random_num_gen_struct, clock()); // sets the seed to the clock meaning the random numbers will be different every time the code is run

    quantities * Quantities = xmalloc(sizeof(quantities));
    Quantities->J=0;
    Quantities->B=0;
    Quantities->temperature=0;
    Quantities->total_energy=0;
    Quantities->delta_e=0;
    Quantities->array_size=0;
    Quantities->number_of_iterations=0;

    int ret_val = 0;
    int option_index = 0;
    int c = getopt_long( argc, argv, ":d:j:b:t:i:", long_options, &option_index );
    /* End of options is signalled with '-1' */
    while (c != -1) {
        switch (c) {
            case 'd':
                //printf("v\n");
                Quantities->array_size = checked_strtoi(optarg);
                break;
            case 'j':
                ret_val = get_double_arg(&Quantities->J, long_options[option_index].name, optarg);
                break;
            case 'b':
                ret_val = get_double_arg(&Quantities->B, long_options[option_index].name, optarg);
                break;
            case 't':
                ret_val = get_double_arg(&Quantities->temperature, long_options[option_index].name, optarg);
                break;
            case 'i':
                Quantities->number_of_iterations = checked_strtol(optarg);
                break;
            case ':':
                /* missing option argument */
                fprintf(stderr, "Error: option '-%c' requires an argument\n", optopt);
                return BAD_ARGS;
            case '?':
                default:
                /* invalid option */
                fprintf(stderr, "Warning: option '-%c' is invalid: ignored\n", optopt);
                break;
        }
        c = getopt_long( argc, argv, ":d:j:b:t:i:", long_options, &option_index );
    }
    int *spin = xmalloc(sizeof(int)*Quantities->array_size*Quantities->array_size);
    populate_array(spin, Quantities, random_num_gen_struct);

    Quantities->total_energy = calculate_energy(spin, Quantities);

    metropolis_implementation(spin, Quantities, random_num_gen_struct);
    FILE * final = fopen("final_array.txt", "w");
    if (final == NULL) {
        printf("Error opening file\n");
        exit(BAD_FILE_OPEN);
    }
    for (int i=0; i<Quantities->array_size; i++) {
        for (int j=0; j<Quantities->array_size; j++) {
            fprintf(final, "%d\t", S(i, j));
        }
        fprintf(final, "\n");
    }
    printf("Energy is : %.18Lg\n", Quantities->total_energy);
    fclose(final);
    xfree(spin);
    xfree(Quantities);

    gsl_rng_free(random_num_gen_struct);

    //gets and prints the total runtime
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    unsigned long long int delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("%llu\n", delta_us);
    return 0;
}

static void * xmalloc(size_t bytes){
    void * retval = malloc(bytes);
    if (retval) {
        return retval;
    }
    printf("memory was not successfully allocated\n");
    exit(BAD_MALLOC);
}

static Error get_double_arg( double *value, const char *opt_name, char *optarg) {
    char * endptr = NULL;
    *value = strtod(optarg, &endptr);
    if (*endptr) {
        printf ("Error: option -%s has an invalid argument `%s'.\n", opt_name, optarg);
        return BAD_ARGS;
    }
    return NO_ERROR;
}

//ensures the value taken by strtoul can be processed as an integer
static int checked_strtoi(const char *string){
    char * endptr;
    errno = 0;
    unsigned long temp_long = strtoul(string, &endptr, BASE_10);
    if (errno == ERANGE || *endptr != '\0' || string == endptr) {
        printf("input dimension size is not an integer\n");
        exit(BAD_ARGS);
    }
    return (int) temp_long;

}

static long checked_strtol(const char *string){
    char * endptr;
    errno = 0;
    unsigned long temp_long = strtoul(string, &endptr, BASE_10);
    if (errno == ERANGE || *endptr != '\0' || string == endptr) {
        printf("input dimension size is not an integer\n");
        exit(BAD_ARGS);
    }
    return (long) temp_long;

}

//randomly populates the array using the Mersenne Twister rng
static Error populate_array(int *spin,  quantities * Quantities, gsl_rng *random_num_gen_struct){
    FILE * output =fopen("output.txt", "w");
    if (output == NULL){
        printf("File failed to open\n");
        exit(BAD_FILE_OPEN);
    }
    double temp =0;
    for (int i=0; i<Quantities->array_size; i++) {
        for (int j=0; j<Quantities->array_size; j++) {
            temp = gsl_rng_uniform(random_num_gen_struct);
            //printf("%f\t", temp);
            if (temp >= 0.5) {
                S(i,j) = 1;
            }
            else{
                S(i,j) = -1;
            }
            fprintf(output, "%d\t", S(i,j));
        }
        fprintf(output,"\n");
    }
    fclose(output);
    return NO_ERROR;
}

static double calculate_energy (int *spin, quantities * Quantities){
    double Energy = 0;
    for (int i=0; i<Quantities->array_size; i++) {
        for (int j=0; j<Quantities->array_size; j++) {
            Energy -= S(i,j)*( Quantities->J*(S(i+1,j)+S(i,j+1)) + MU_0*Quantities->B );
        }
    }
    return Energy;
}

static double calculate_energy_change (int *spin, quantities * Quantities, unsigned long i, unsigned long j){
    double delta_energy =  S(i,j)*( 2.0*Quantities->J*(S(i-1,j)+S(i,j-1)+S(i+1,j)+S(i,j+1)) + MU_0*Quantities->B );;
    return delta_energy;
}

static Error metropolis_implementation (int *spin, quantities * Quantities, gsl_rng * rng_struct){
    unsigned long i=0, j=0, x=0;
    int count =0;
    long double new_energy_val=0;
    long double energy_change=0;
    double random=0;
    FILE * energy = fopen("energy_fluctuations.txt", "w");
    if (energy == NULL) {
           printf("Error opening file\n");
           exit(BAD_FILE_OPEN);
       }
    while (count<Quantities->number_of_iterations) {

        if (count%10 == 0) {
            Quantities->total_energy = calculate_energy(spin, Quantities);
        }
        for (x=0; x<Quantities->array_size*Quantities->array_size; x++) {
            i = gsl_rng_uniform_int(rng_struct, Quantities->array_size);
            j = gsl_rng_uniform_int(rng_struct, Quantities->array_size);
            Quantities->delta_e = calculate_energy_change(spin, Quantities, i, j);
            if (Quantities->delta_e <= 0.0) {
                S(i,j) = -S(i, j);
            }
            else{
                random = gsl_rng_uniform(rng_struct);
                if (random < exp(-(Quantities->delta_e)/(K_B * Quantities->temperature))) {
                    S(i, j) = -S(i, j);
                }
            }
        }
        if (count%10 == 0) {
            new_energy_val = calculate_energy(spin, Quantities);
            energy_change = new_energy_val - Quantities->total_energy;
            int f = fprintf(energy, "At step %d energy fluctuation is: %.18Lg\n", count, energy_change);
            printf("f = %d\n", f);
            if (f <= 0){
                exit(1);
            }
            printf("At step %d energy fluctuation is: %.18Lg\n", count, energy_change);
            printf("Here\n");
        }
        count++;
        printf("%d\n", count);
    }
    fclose(energy);
    return NO_ERROR;
}
