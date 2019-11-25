//
//  main.c
//  N-Body
//
//  Created by George Jerman on 17/11/2019.
//  Copyright © 2019 George Jerman. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#define MAX_BODY_NAME_LENGTH 20
#define ARRAY_SIZE_FOR_COORDS 4
#define MAX_FILE_NAME_LENGTH 50
#define MAX_LINE_LENGTH 500
#define MAX_COLS 8
#define BIG_G 6.67430e-11
#define VERLET 0
#define RK4 1
#define UNSET 99

typedef enum {
    NO_ERROR = 0,
    NO_MEMORY = 1,
    BAD_ARGS = 2,
    BAD_FILENAME = 3,
    BAD_FORMAT = 5,
    UNKNOWN_ERROR = -1
} Error;

typedef enum coords{X,Y,Z, NUM_OF_COORDS} Coords;

typedef struct Body{
    double mass;
    double position[NUM_OF_COORDS];
    double velocity[NUM_OF_COORDS];
    double acceleration[NUM_OF_COORDS];
    char name[MAX_BODY_NAME_LENGTH];
} Body;
static Error get_long_arg( long *value, const char *opt_name, char *optarg);
static int size_of_file(FILE *input);
static Error populate_arrays(FILE * input, long number_of_bodies, Body *body);
static Error verlet_integration(Body *bodies,unsigned int body_num, long time_step, long final_time, FILE * output, FILE * energy_output);
static double calculate_acceleration(Body * bodies, unsigned int current_body,unsigned int total_bodies, Coords dimension, FILE * output);
static Error get_total_energy(int current_time, Body * bodies, int number_of_bodies, Coords dimension, long final_time, long time_step, FILE * energy_output);
static double get_kinetic_energy(Body * bodies, int number_of_bodies);
static double get_potential_energy(Body * bodies, int number_of_bodies, int current_body, Coords dimension);

int main(int argc, char ** argv)
{

    char version_num[] = "Version 0.1 Date modified - 18/11/19";
    
    int ret_val=0;
    int verlet_or_rk4 = UNSET;
    char * filename;
    FILE * input;

    
    static struct option long_options[] = {
        /* These options don’t set a flag the are edistinguished by their indices. */
        {"verlet", no_argument,  0, 'v'},
        {"RK4",  no_argument,  0, 'r'},
        {"time", required_argument, 0, 't'},
        {"final time", required_argument, 0, 'f'},
        {0, 0, 0, 0}
        };
        
    /* getopt_long needs somewhere to store its option index. */
    int option_index = 0;
    unsigned int number_of_bodies;
    long time_step = 0;
    long final_time = 0;
    FILE * output = fopen("output.txt", "w");
    FILE * energy_output = fopen("Energy.txt", "w");
    printf("# %s\n", version_num);
    
    int c = getopt_long( argc, argv, ":vrt:f:", long_options, &option_index );
    /* End of options is signalled with '-1' */
    while (c != -1) {
        switch (c) {
            case 'v':
                //printf("v\n");
                verlet_or_rk4 = 0;
                break;
            case 'r':
                //printf("r\n");
                verlet_or_rk4 = 1;
                break;
            case 't':
                ;
                get_long_arg( &time_step, long_options[option_index].name, optarg);
                //printf("%d\n", time_step);
                break;
            case 'f':
                ;
                get_long_arg( &final_time, long_options[option_index].name, optarg);
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
        c = getopt_long( argc, argv, ":vrt:f:", long_options, &option_index );
    }
    int too_many_args =0;
    for(;optind<argc;optind++) {
        if (too_many_args == 0) {
            filename = argv[optind];
            //printf("%s\n", filename);
            too_many_args++;
        }
        else{
            printf("only one filename can be specified\n");
            exit(BAD_FILENAME);
        }
    }
    input = fopen(filename, "r");
    if(input == NULL){
        perror("fopen");
        exit(BAD_FILENAME);
    }
    number_of_bodies = size_of_file(input);
    //printf("%d\n", number_of_bodies);
    //allocating space for the bodies
    Body * bodies = malloc(sizeof(Body)*number_of_bodies);
    populate_arrays(input, number_of_bodies, bodies);
    if (verlet_or_rk4 == VERLET) {
        verlet_integration(bodies, number_of_bodies, time_step, final_time, output, energy_output);
        //get_total_energy(bodies, number_of_bodies, NUM_OF_COORDS, final_time, time_step, energy_output);

    }
    if (verlet_or_rk4 == RK4) {
        //do RK4
        ;
    }
    if (verlet_or_rk4 == UNSET) {
        printf("Error in choosing method\n");
        exit(BAD_ARGS);
    }

    //printf("SUN: %lg\t%lg\t%lg\n", bodies[0].position[X], bodies[0].position[Y], bodies[0].position[Z]);
    //printf("EARTH: %lg\t%lg\t%lg\n", bodies[1].position[X], bodies[1].position[Y], bodies[1].position[Z]);
    free(bodies);
    fclose(output);
    fclose(energy_output);
    return NO_ERROR;
}

/* Read an argument of type 'long' */
static Error get_long_arg( long *value, const char *opt_name, char *optarg) {
    char * endptr = NULL;
    *value = strtol(optarg, &endptr, 10);
    if (*endptr) {
        printf ("Error: option -%s has an invalid argument `%s'.\n", opt_name, optarg);
        return BAD_ARGS;
    }
    return NO_ERROR;
}

//reads number of bodies in the file ignoring comment lines (ones beginning with a hash)
static int size_of_file(FILE *input)
{
    int nl=0;
    char buffer[MAX_LINE_LENGTH];
    fseek(input, 0, SEEK_SET);
    while ((fgets(buffer, MAX_LINE_LENGTH, input)) !=NULL)
    {
        //ignore comment lines
        if (buffer[0] == '#') {
            continue;
        }
        //checks for any sneaky newlines at end of file or generally within file
        if (buffer[0] == '\n') {
            continue;
        }
        nl++;
        //printf("%d\n",nl);
    }
    fseek(input, 0, SEEK_SET);
    return nl;
}

//fills the arrays from the data
static Error populate_arrays(FILE * input, long number_of_bodies, Body *body){
    int i,f;
    int body_number = 0;
    char buffer[MAX_LINE_LENGTH];
    char name_buffer[MAX_BODY_NAME_LENGTH];
    while(fgets(buffer, MAX_LINE_LENGTH, input) != NULL){
        if (buffer[0]=='#') {
            break;
        }
        else{
            printf("File not of expected format\n");
            exit(BAD_FORMAT);
        }
    }
    for (i = 0; i<number_of_bodies; i++) {
        fgets(buffer, MAX_LINE_LENGTH, input);
        f = sscanf(buffer, "%s %lg %lg %lg %lg %lg %lg %lg", name_buffer, &body[body_number].mass, &body[body_number].position[X], &body[body_number].position[Y], &body[body_number].position[Z], &body[body_number].velocity[X], &body[body_number].velocity[Y], &body[body_number].velocity[Z]);
        if(f != MAX_COLS){
            printf("Number of columns read does not match the expected number in line %d\n", i+2); //+2 to account for comment line and so the first line is 1 instead of 0
            exit(BAD_FORMAT);
        }
        strncpy(body[body_number].name, name_buffer, MAX_BODY_NAME_LENGTH);
        //printf("%s\t %lg\t %lg\t %lg\t %lg\t %lg\t %lg\t %lg\n", name_buffer, body[body_number].mass, body[body_number].position[X], body[body_number].position[Y], body[body_number].position[Z], body[body_number].velocity[X], body[body_number].velocity[Y], body[body_number].velocity[Z]);
        body_number++;
    
    }
    for (int i =0; i<number_of_bodies; i++) {
            printf("%s %lg %lg %lg %lg %lg %lg %lg\n", body[i].name, body[i].mass, body[i].position[X], body[i].position[Y], body[i].position[Z], body[i].velocity[X], body[i].velocity[Y], body[i].velocity[Z]);
    }
    fclose(input);
    return NO_ERROR;
}

static Error verlet_integration(Body *bodies, unsigned int body_num, long time_step, long final_time, FILE * output, FILE * energy_output){
    unsigned int i,j,k;
    //double pot_en=0;
    //double kin_en=0;
    //double tot_en=0;
    if (time_step == 0 || time_step < 0){
        printf("Time step must be a positive non-zero number\n");
        exit(BAD_ARGS);
    }
    if (final_time == 0 || final_time < 0) {
        printf("Stop time must be a positive non-zero number\n");
        exit(BAD_ARGS);
    }
    if (final_time % time_step !=0) {
        printf("The runtime given must be divisible by the step given\n");
        exit(BAD_ARGS);
    }
    for (i=0; i<final_time; i += time_step) {
        
        for (j=0; j<body_num; j++) {
            for (k=0; k<NUM_OF_COORDS; k++) {
                bodies[j].acceleration[k] = calculate_acceleration(bodies, j,body_num, k, output);
            }
            for (k=0; k<NUM_OF_COORDS; k++) {
                bodies[j].velocity[k] += (0.5 * bodies[j].acceleration[k] * time_step); //intermidate velocity
            }
            for (k=0; k<NUM_OF_COORDS; k++) {
                bodies[j].position[k] += bodies[j].velocity[k]*time_step; //new position
                fprintf(output, "%lg\t", bodies[j].position[k]);
            }
            fprintf(output, "\t");
            for (k=0; k<NUM_OF_COORDS; k++) {
                bodies[j].acceleration[k] = calculate_acceleration(bodies, j,body_num, k, output);
            }
            for (k=0; k<NUM_OF_COORDS; k++) {
                bodies[j].velocity[k] += 0.5*bodies[j].acceleration[k]*time_step; //new velocity
            }
            get_total_energy(i, bodies, body_num, NUM_OF_COORDS, final_time, time_step, energy_output);
        }
        fprintf(output, "\n");
    }
    return NO_ERROR;
}

static double calculate_acceleration(Body * bodies, unsigned int current_body, unsigned int total_bodies, Coords dimension, FILE * output){
    //fprintf(output, "%lg\t%lg\t%lg\t\n\n", bodies[0].acceleration[X], bodies[0].acceleration[Y], bodies[0].acceleration[Z]);
    int i,j;
    double accel = 0.0;
    double distance = 0.0;
    for (i=0; i<total_bodies; i++) {
        if (i != current_body) {
            distance=0.0;
            for (j=0; j<NUM_OF_COORDS; j++) {
                distance += pow(bodies[current_body].position[j]-bodies[i].position[j], 2);
            }
            distance = sqrt(distance);
            accel += -(BIG_G * bodies[i].mass * (bodies[current_body].position[dimension]-bodies[i].position[dimension])) /pow(distance,3);
        }
    }
    return accel;
}
static Error get_total_energy(int current_time, Body * bodies, int number_of_bodies, Coords dimension, long final_time, long time_step, FILE * energy_output){
    int j,k;
    
    double kinetic_energy = 0;
    double potential_energy = 0;
    double total_energy = 0;
    double temp =0;
        kinetic_energy = get_kinetic_energy(bodies, number_of_bodies);
        for (j=0; j<number_of_bodies; j++) {
            for (k=0; k<NUM_OF_COORDS; k++) {
                temp += get_potential_energy(bodies, number_of_bodies, j, k);
            }
            potential_energy += temp;
        }
        //potential_energy = fabs(potential_energy);
        total_energy = kinetic_energy + potential_energy;
        fprintf(energy_output, "%d\t%lg\t%lg\t%lg\n",current_time, kinetic_energy, potential_energy, total_energy);
    return NO_ERROR;
}

static double get_kinetic_energy(Body * bodies, int number_of_bodies){
    int i;
    double kinetic_energy=0;
    int total_velocity=0;
    for (i = 0; i<number_of_bodies; i++) {
        total_velocity = sqrt(pow(bodies[i].velocity[X],2) + pow(bodies[i].velocity[Y], 2) + pow(bodies[i].velocity[Z], 2));
        kinetic_energy += 0.5 * bodies[i].mass * pow(total_velocity, 2);
    }
    return kinetic_energy;
}

static double get_potential_energy(Body * bodies, int number_of_bodies, int current_body, Coords dimension){
    int i,j;
    double pot_en = 0.0;
    double distance = 0.0;
    for (i=0; i<number_of_bodies; i++) {
        if (i != current_body) {
            distance=0.0;
            for (j=0; j<NUM_OF_COORDS; j++) {
                distance += pow(bodies[current_body].position[j]-bodies[i].position[j], 2);
            }
            distance = sqrt(distance);
            pot_en += (BIG_G * bodies[i].mass* bodies[current_body].mass /distance);
        }
    }
    return pot_en;
}
