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
#define DIMENSIONS 3

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

typedef enum coords{X,Y,Z} Coords;

typedef struct Body{
    double mass;
    double position[DIMENSIONS];
    double velocity[DIMENSIONS];
    double acceleration[DIMENSIONS];
    char name[MAX_BODY_NAME_LENGTH];
} Body;

int size_of_file(FILE *input);
int populate_arrays(FILE * input, long number_of_bodies, Body *body);
static Error verlet_integration(Body *bodies,unsigned int body_num, unsigned int time_step, unsigned int final_time);

int main(int argc, char ** argv)
{

    char version_num[] = "Version 0.1 Date modified - 18/11/19";
    
    int ret_val=0;
    int verlet_or_rk4;
    char * filename;
    FILE * input;

    
    static struct option long_options[] = {
        /* These options don’t set a flag the are edistinguished by their indices. */
        {"verlet",  required_argument,  0, 'v'},
        {"RK4",  required_argument,  0, 'r'},
        {"time", required_argument, 0, 't'},
        {"final time", required_argument, 0, 'f'},
        {0, 0, 0, 0}
        };
        
    /* getopt_long needs somewhere to store its option index. */
    int option_index = 0;
    unsigned int number_of_bodies;
    unsigned int time_step = 0;
    unsigned int final_time = 0;
    
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
                time_step = atoi(optarg);
                //printf("%d\n", time_step);
                break;
            case 'f':
                ;
                final_time = atoi(optarg);
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
    printf("%d\n", number_of_bodies);
    //allocating space for the bodies
    Body * bodies = malloc(sizeof(Body)*number_of_bodies);
    populate_arrays(input, number_of_bodies, bodies);
    verlet_integration(bodies, number_of_bodies, time_step, final_time);
    free(bodies);
    return NO_ERROR;
}

//reads number of bodies in the file ignoring comment lines (ones beginning with a hash)
int size_of_file(FILE *input)
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
int populate_arrays(FILE * input, long number_of_bodies, Body *body){
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
        printf("%s\t %lg\t %lg\t %lg\t %lg\t %lg\t %lg\t %lg\n", name_buffer, body[body_number].mass, body[body_number].position[X], body[body_number].position[Y], body[body_number].position[Z], body[body_number].velocity[X], body[body_number].velocity[Y], body[body_number].velocity[Z]);
        body_number++;
    
    }
    fclose(input);
    return NO_ERROR;
}

static Error verlet_integration(Body *bodies, unsigned int body_num, unsigned int time_step, unsigned int final_time){
    unsigned int i,j;
    double new_pos_x = 0, new_pos_y = 0, new_pos_z = 0;
    double new_accel_x = 0, new_accel_y = 0, new_accel_z = 0;
    double new_vel_x = 0, new_vel_y = 0, new_vel_z = 0;
    if (time_step == 0){
        printf("Time step must be non-zero\n");
        exit(BAD_ARGS);
    }
    for (i=0; i<final_time; i += time_step) {
        for (j=0; j<body_num; j++) {
            new_pos_x = bodies[j].position[X] + bodies[j].velocity[X] + (bodies[j].acceleration[X]*time_step*time_step*0.5);
            new_pos_y = bodies[j].position[Y] + bodies[j].velocity[Y] + (bodies[j].acceleration[Y]*time_step*time_step*0.5);
            new_pos_z = bodies[j].position[Z] + bodies[j].velocity[Z] + (bodies[j].acceleration[Z]*time_step*time_step*0.5);
            
            new_accel_x = bodies[j].acceleration[X]; //replace with gravity once done
            new_accel_y = bodies[j].acceleration[Y]; //replace with gravity once done
            new_accel_z = bodies[j].acceleration[Z]; //replace with gravity once done
            
            new_vel_x = bodies[j].velocity[X] +(bodies[j].acceleration[X] + new_accel_x) * (time_step * 0.5);
            new_vel_y = bodies[j].velocity[Y] +(bodies[j].acceleration[Y] + new_accel_y) * (time_step * 0.5);
            new_vel_z = bodies[j].velocity[Z] +(bodies[j].acceleration[Z] + new_accel_z) * (time_step * 0.5);
            
            bodies[j].position[X] = new_pos_x;
            bodies[j].position[Y] = new_pos_y;
            bodies[j].position[Z] = new_pos_z;
            
            bodies[j].velocity[X] = new_vel_x;
            bodies[j].velocity[Y] = new_vel_y;
            bodies[j].velocity[Z] = new_vel_z;
            
            bodies[j].acceleration[X] = new_accel_x;
            bodies[j].acceleration[Y] = new_accel_y;
            bodies[j].acceleration[Z] = new_accel_z;
            
        }
    }
    return NO_ERROR;
}
