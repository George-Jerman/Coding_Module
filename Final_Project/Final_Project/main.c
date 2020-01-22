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
#include <stdbool.h>
#include <errno.h>
#include <gsl/gsl_rng.h>
#include <time.h>

/*---------definitions-----------*/
#define BASE_10 10
#define xfree(mem) { free( mem ); mem = NULL; } //checks if free is successful
#define S(i,j) (spin[((array_size+(i))%array_size)*array_size+((array_size+(j))%array_size)]) /* Makes referring to the array simpler and also deals
                                                                                                with the periodic BCs required to simulate an infinite lattice
                                                                                                i.e this means that the N * N array element wraps around to the 0th element*/

typedef enum {
    NO_ERROR = 0,
    NO_MEMORY = 1,
    BAD_ARGS = 2,
    BAD_FILENAME = 3,
    BAD_FORMAT = 5,
    UNKNOWN_ERROR = -1,
    BAD_MALLOC = 100
} Error;

struct timespec start, end;

/*-----------prototypes-----------*/
void * xmalloc(size_t bytes);
int checked_strtoi(const char *string);
Error populate_array(bool *spin, int array_size, gsl_rng *r);

int main(int argc, char ** argv) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    static struct option long_options[] = {
    /* These options don’t set a flag the are edistinguished by their indices. */
    {"matrix_size", no_argument,  0, 'd'},
    {0, 0, 0, 0}
    };
    //tells the program to use the mersenne twister rng algorithm.
    gsl_rng * random_num_gen_struct = gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set(random_num_gen_struct, clock()); // randomises the seed every time the code is run
    
    int array_size = 0;
    int option_index = 0;
    int c = getopt_long( argc, argv, ":d:", long_options, &option_index );
    /* End of options is signalled with '-1' */
    while (c != -1) {
        switch (c) {
            case 'd':
                //printf("v\n");
                array_size = checked_strtoi(optarg);
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
        c = getopt_long( argc, argv, ":vrt:f:", long_options, &option_index );
    }
    //bool array used to minimise the memory footprint
    bool *spin = xmalloc(sizeof(bool)*array_size*array_size);
    populate_array(spin, array_size, random_num_gen_struct);
    xfree(spin);
    //printf("%d", ((array_size+(10))%array_size)*array_size+((array_size+(6))%array_size));
    gsl_rng_free(random_num_gen_struct);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    u_int64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("%llu\n", delta_us);
    return 0;
}

void * xmalloc(size_t bytes){
    void * retval = malloc(bytes);
    if (retval) {
        return retval;
    }
    printf("memory was not successfully allocated\n");
    exit(BAD_MALLOC);
}

//ensures the value taken by strtoul can be processed as an integer
int checked_strtoi(const char *string){
    char * endptr;
    errno = 0;
    unsigned long temp_long = strtoul(string, &endptr, BASE_10);
    if (errno == ERANGE || *endptr != '\0' || string == endptr) {
        printf("input dimension size is not an integer\n");
        exit(BAD_ARGS);
    }
    return (int) temp_long;
    
}

//randomly populates the array using the Mersenne Twister rng
Error populate_array(bool *spin, int array_size, gsl_rng *random_num_gen_struct){
    FILE * output =fopen("output.txt", "w");
    double temp =0;
    for (int i=0; i<array_size; i++) {
        for (int j=0; j<array_size; j++) {
            temp = gsl_rng_uniform(random_num_gen_struct);
            //printf("%f\t", temp);
            if (temp >= 0.5) {
                S(i,j) = true;
            }
            else{
                S(i,j) = false;
            }
            fprintf(output, "%d\t", S(i,j));
        }
        fprintf(output,"\n");
    }
    fclose(output);
    return NO_ERROR;
}
