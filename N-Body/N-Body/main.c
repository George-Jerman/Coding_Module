//
//  main.c
//  N-Body
//
<<<<<<< HEAD
//  Created by George Jerman on 17/11/2019.
//  Version 1.0 12/12/19 - First fully working version of both Verlet integration and RK4 methods.
=======
//Created by George Jerman on 17/11/2019.
//
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
//
/*  General command line argument code taken from https://vle.exeter.ac.uk/mod/resource/view.php?id=961497
//  Author: C.D.H.Williams and adapted by George Jerman for use in this project under Public Domain
*/
<<<<<<< HEAD
/*
    Manual
 -----------------------------------
 This program takes in input of format:
 
 # Name    Mass    x    y    z    Vx    Vy    Vz
 Sun    2e30    0    0    0    0    0    0
 Earth    6e24    149598023000    0.0    0.0    0.0    30000    0.0
 Moon    7.3e22  149982422000    0.0     0.0     0.0     31000   0.0
 
 and takes its arguments in the form of:
 
 ./PROG_NAME -v -t 1 -f 1000 inputdata.txt
 
 which runs the program called PROG_NAME using the verlet method (-r is used to RK4) with a time step of 1
 up to a final time of 1000. The arguments can be given in any order except the input file which must be given last.
 
 The program outputs the positions of the bodies at each time step in the form of
 body0_x body0_y body0_z body1_x body1_y body1_z body2_x body2_y body2_z
 in a tab separated manner which allows each body to be plotted using Gnuplot.
 
 The energies are plotted in a file called Energy.txt of the format:
 Time Kinetic GPE Total
 
 for each time step.
 */
=======
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#include <sys/time.h>

#define MAX_BODY_NAME_LENGTH 20
#define MAX_FILE_NAME_LENGTH 50
#define MAX_LINE_LENGTH 500
#define MAX_COLS 8
#define BIG_G 6.67430e-11
#define VERLET 0
#define RK4 1
#define UNSET 99
<<<<<<< HEAD
#define NUM_OF_PARAMS 2
#define xfree(mem) { free( mem ); mem = NULL; } //checks if free is successful
=======
#define NUM_OF_PARAMS 3
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713

typedef enum {
    NO_ERROR = 0,
    NO_MEMORY = 1,
    BAD_ARGS = 2,
    BAD_FILENAME = 3,
    BAD_FORMAT = 5,
    UNKNOWN_ERROR = -1,
    BAD_MALLOC = 100
} Error;

typedef enum coords{X,Y,Z, NUM_OF_COORDS} Coords;

typedef struct Body{
    double position[NUM_OF_COORDS];
    double velocity[NUM_OF_COORDS];
    double acceleration[NUM_OF_COORDS];
    double mass;
    char name[MAX_BODY_NAME_LENGTH];
<<<<<<< HEAD
    unsigned int number_of_bodies;
    int current_body;
} Body;

=======
} Body;

struct param_type {
    double position[NUM_OF_COORDS];
    double velocity[NUM_OF_COORDS];
    double acceleration[NUM_OF_COORDS];
    double mass;
    unsigned int number_of_bodies;
};
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713

struct timespec start, end;

static Error get_long_arg( unsigned long *value, const char *opt_name, char *optarg);
void * xmalloc(size_t bytes);
static unsigned int size_of_file(FILE *input);
<<<<<<< HEAD
static Error populate_arrays(FILE * input, unsigned int number_of_bodies, Body *body);
static Error verlet_integration(Body *bodies, unsigned long time_step, unsigned long final_time, FILE * output, FILE * energy_output);
static double calculate_acceleration(Body *bodies, unsigned  int current_body, Coords dimension);
static long get_total_energy(unsigned long current_time, Body * bodies, FILE * energy_output);
static double get_kinetic_energy(Body *bodies, unsigned int number_of_bodies);
static double get_potential_energy(Body *bodies, unsigned int current_body);
static int function (double t, const double x[], double y[], void *params);
=======
static Error populate_arrays(FILE * input, long number_of_bodies, Body *body);
static Error verlet_integration(Body *bodies,unsigned  int body_num, unsigned long time_step, unsigned long final_time, FILE * output, FILE * energy_output);
static double calculate_acceleration(Body * bodies, unsigned  int current_body,unsigned  int total_bodies, Coords dimension);
static long get_total_energy(unsigned long current_time, Body * bodies, unsigned int number_of_bodies, FILE * energy_output);
static double get_kinetic_energy(Body * bodies, unsigned int number_of_bodies);
static double get_potential_energy(Body * bodies, unsigned int number_of_bodies, unsigned int current_body);
int function (double t, const double x[], double y[], struct param_type * my_params);
static double calculate_accelerationRK4(struct param_type * my_params, unsigned  int current_body, Coords dimension);
static int three_d_to_one_d_index(unsigned int num_bodies, int i, int j, int k);

>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713

int main(int argc, char ** argv)
{

<<<<<<< HEAD
    char version_num[] = "Version 1.0 Date modified - 12/12/19";
=======
    char version_num[] = "Version 0.1 Date modified - 18/11/19";
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
    
    int verlet_or_rk4 = UNSET;
    char * filename = NULL;
    FILE * input;

<<<<<<< HEAD
=======
    
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
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
    unsigned  int number_of_bodies;
    unsigned long time_step = 0;
    unsigned long final_time = 0;
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
                return BAD_ARGS;
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
            if (strlen(argv[optind]) <= MAX_FILE_NAME_LENGTH) {
                filename = argv[optind];
                too_many_args++;
            }
            else{
                printf("Filename length must be less than 50 characters\n");
                exit(BAD_FILENAME);
            }
            //printf("%s\n", filename);
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
<<<<<<< HEAD
    //allocating space for the bodies
    Body * bodies = xmalloc(sizeof(Body)*number_of_bodies);
    populate_arrays(input, number_of_bodies, bodies);
    bodies[0].number_of_bodies =number_of_bodies;
    if (verlet_or_rk4 == VERLET) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        verlet_integration(bodies, time_step, final_time, output, energy_output);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
       printf("Tine: %llu", delta_us);
    }
    if (verlet_or_rk4 == RK4) {
        
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        unsigned int i,j,k;
        //number of ODEs to solve
        int dimension = NUM_OF_COORDS*NUM_OF_PARAMS*number_of_bodies;
        printf("Dimension = %d\n", dimension);
        double eps_abs = 1e-6;
        double eps_rel = 1e-6;
        unsigned long number_of_steps = final_time/time_step;
        double t_start = 0;

        double y[dimension];
        gsl_odeiv2_system sys = {function, NULL, dimension, bodies};
        gsl_odeiv2_driver * d =gsl_odeiv2_driver_alloc_y_new (&sys, gsl_odeiv2_step_rk4,time_step,eps_abs, eps_rel);
        /*array structure like so:
         [body_0 position, body_0 velocity, body_1 position, body_1 velocity etc etc]
        */
        for (i=0; i<number_of_bodies; i++) {
            for (j=0; j<NUM_OF_COORDS; j++) {
                y[i*(NUM_OF_PARAMS*NUM_OF_COORDS)+j] = bodies[i].position[j];
                y[i*(NUM_OF_PARAMS*NUM_OF_COORDS)+j+NUM_OF_COORDS] = bodies[i].velocity[j];
            }
        }
        
        gsl_odeiv2_driver_set_nmax(d, number_of_steps);
        for (i=0; i<final_time; i+=time_step){
            int status = gsl_odeiv2_driver_apply(d, &t_start, (t_start+time_step), y);
            if (status != GSL_SUCCESS)
            {
                printf ("error, return value=%d\n", status);
                break;
            }
            get_total_energy(i, bodies, energy_output);
            for (j=0; j<number_of_bodies;j++) {
                for (k=0; k<NUM_OF_COORDS; k++) {
                    fprintf(output, "%lg\t", y[j*(NUM_OF_PARAMS*NUM_OF_COORDS)+k]);
                }
            }
            fprintf(output, "\n");
        }
        fprintf(output, "\n");
    gsl_odeiv2_driver_free(d);
    xfree(bodies);
    fclose(output);
    fclose(energy_output);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("Tine: %llu", delta_us);
=======
    //printf("%d\n", number_of_bodies);
    //allocating space for the bodies
    Body * bodies = xmalloc(sizeof(Body)*number_of_bodies);
    populate_arrays(input, number_of_bodies, bodies);
    if (verlet_or_rk4 == VERLET) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        verlet_integration(bodies, number_of_bodies, time_step, final_time, output, energy_output);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        //get_total_energy(bodies, number_of_bodies, NUM_OF_COORDS, final_time, time_step, energy_output);
        uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
        printf("Tine: %llu", delta_us);
    }
    if (verlet_or_rk4 == RK4) {
        unsigned int i,j,k;
        
        struct param_type my_params;
        const double time_step_RK4 = time_step;
        unsigned long number_of_steps = final_time/time_step_RK4;
        gsl_odeiv2_system sys = { function,NULL, NUM_OF_COORDS, &my_params};
        gsl_odeiv2_driver * d = gsl_odeiv2_driver_alloc_y_new (&sys, gsl_odeiv2_step_rk4,time_step_RK4, 1e-5, 1e-5);
        
        double t = 0.0;
        int size_of_y = NUM_OF_PARAMS*NUM_OF_COORDS*number_of_bodies;
        printf("%d",size_of_y );
        double y[size_of_y];
        for (i=0; i<number_of_bodies; i++) {
            for (j=0; j<NUM_OF_PARAMS; j++) {
                for (k=0; k<NUM_OF_COORDS; k++) {
                    if (j==0) {
                        y[three_d_to_one_d_index(number_of_bodies, i, j, k)] = bodies[i].position[k];
                    }
                    if (j==1) {
                        y[three_d_to_one_d_index(number_of_bodies, i, j, k)]= bodies[i].acceleration[k];
                    }
                    
                }
            }
        }
        
        for (i = 0; i < final_time; i +=time_step_RK4){
              for (j=0; j<number_of_bodies; j++) {
                  my_params.position[X] = bodies[j].position[X];
                  my_params.position[Y] = bodies[j].position[Y];
                  my_params.position[Z] = bodies[j].position[Z];
                  my_params.velocity[X] = bodies[j].velocity[X];
                  my_params.velocity[Y] = bodies[j].velocity[Y];
                  my_params.velocity[Z] = bodies[j].velocity[Z];
                  my_params.acceleration[X] = bodies[j].acceleration[X];
                  my_params.acceleration[Y] = bodies[j].acceleration[Y];
                  my_params.acceleration[Z] = bodies[j].acceleration[Z];
                  my_params.mass=bodies->mass;
                  my_params.number_of_bodies = number_of_bodies;
                  for (k=0; k<NUM_OF_COORDS; k++) {
                      int s = gsl_odeiv2_driver_apply_fixed_step(d, &t,time_step_RK4 ,number_of_steps, y);
                      if (s != GSL_SUCCESS)
                      {
                          printf ("error: driver returned %d\n", s);
                          break;
                      }
                  }
                  fprintf(output, "%lg\t%lg\t%lg", y[three_d_to_one_d_index(number_of_bodies, j, 0, 0)], y[three_d_to_one_d_index(number_of_bodies, j, 0, 1)], y[three_d_to_one_d_index(number_of_bodies, j, 0, 2)]);
              }
              fprintf(output, "\n");
        }
    gsl_odeiv2_driver_free (d);
    free(bodies);
    fclose(output);
    fclose(energy_output);
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
    }
    if (verlet_or_rk4 == UNSET) {
        printf("Error in choosing method\n");
        exit(BAD_ARGS);
    }
    return NO_ERROR;
}
<<<<<<< HEAD
    
=======
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713

//-------------------------------------------------------------------------------------------------------------------------
//General functions for both verlet and rk4 methods

/* Read an argument of type 'long' */
static Error get_long_arg( unsigned long *value, const char *opt_name, char *optarg) {
    char * endptr = NULL;
    *value = strtoul(optarg, &endptr, 10);
    if (*endptr) {
        printf ("Error: option -%s has an invalid argument `%s'.\n", opt_name, optarg);
        return BAD_ARGS;
    }
    return NO_ERROR;
}

void * xmalloc(size_t bytes){
    void * retval = malloc(bytes);
    if (retval) {
        return retval;
    }
    printf("memory was not successfully allocated\n");
    exit(BAD_MALLOC);
}

//reads number of bodies in the file ignoring comment lines (ones beginning with a hash)
static unsigned int size_of_file(FILE *input)
{
    unsigned int nl=0;
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
<<<<<<< HEAD
static Error populate_arrays(FILE * input, unsigned int number_of_bodies, Body *bodies){
    unsigned int i;
    int f;
    int line =1;
    unsigned int body_number = 0;
=======
static Error populate_arrays(FILE * input, long number_of_bodies, Body *body){
    int i,f;
    int line =1;
    int body_number = 0;
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
    char buffer[MAX_LINE_LENGTH];
    char name_buffer[MAX_BODY_NAME_LENGTH];
    while(fgets(buffer, MAX_LINE_LENGTH, input) != NULL){
        if (buffer[0]=='#') {
            line++;
        }
        else if (body_number< number_of_bodies){
<<<<<<< HEAD
            f = sscanf(buffer, "%s %lg %lg %lg %lg %lg %lg %lg", name_buffer, &bodies[body_number].mass, &bodies[body_number].position[X], &bodies[body_number].position[Y], &bodies[body_number].position[Z], &bodies[body_number].velocity[X], &bodies[body_number].velocity[Y], &bodies[body_number].velocity[Z]);
=======
            f = sscanf(buffer, "%s %lg %lg %lg %lg %lg %lg %lg", name_buffer, &body[body_number].mass, &body[body_number].position[X], &body[body_number].position[Y], &body[body_number].position[Z], &body[body_number].velocity[X], &body[body_number].velocity[Y], &body[body_number].velocity[Z]);
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
            if(f != MAX_COLS){
                printf("Number of columns read does not match the expected number in line %d\n", line);
                exit(BAD_FORMAT);
            }
<<<<<<< HEAD
            strncpy(bodies[body_number].name, name_buffer, MAX_BODY_NAME_LENGTH);
=======
            strncpy(body[body_number].name, name_buffer, MAX_BODY_NAME_LENGTH);
            //printf("%s\t %lg\t %lg\t %lg\t %lg\t %lg\t %lg\t %lg\n", name_buffer, body[body_number].mass, body[body_number].position[X], body[body_number].position[Y], body[body_number].position[Z], body[body_number].velocity[X], body[body_number].velocity[Y], body[body_number].velocity[Z]);
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
            line++;
            body_number++;
        }
        else if (body_number==number_of_bodies-1){
            continue;
        }
        else{
            printf("Bad format, possibly trailing new line\n");
            exit(BAD_FORMAT);
        }
    }
    for (i =0; i<number_of_bodies; i++) {
<<<<<<< HEAD
        printf("%s %lg %lg %lg %lg %lg %lg %lg\n", bodies[i].name, bodies[i].mass, bodies[i].position[X], bodies[i].position[Y], bodies[i].position[Z], bodies[i].velocity[X], bodies[i].velocity[Y], bodies[i].velocity[Z]);
        bodies[i].number_of_bodies= number_of_bodies;
=======
            printf("%s %lg %lg %lg %lg %lg %lg %lg\n", body[i].name, body[i].mass, body[i].position[X], body[i].position[Y], body[i].position[Z], body[i].velocity[X], body[i].velocity[Y], body[i].velocity[Z]);
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
    }
    fclose(input);
    return NO_ERROR;
}

//-------------------------------------------------------------------------------------------------------------------------------------------
//Functions for use in Verlet

<<<<<<< HEAD
static Error verlet_integration(Body *bodies,unsigned long time_step,unsigned long final_time, FILE * output, FILE * energy_output){
=======
static Error verlet_integration(Body *bodies, unsigned  int body_num,unsigned long time_step,unsigned long final_time, FILE * output, FILE * energy_output){
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
    unsigned long i;
    unsigned int j,k;
    
    //error checking of time inputs
    if (time_step == 0){
        printf("Time step must be a non-zero number\n");
        exit(BAD_ARGS);
    }
    if (final_time == 0) {
        printf("Stop time must be a non-zero number\n");
        exit(BAD_ARGS);
    }
    if (final_time % time_step !=0) {
        printf("The runtime given must be divisible by the step given\n");
        exit(BAD_ARGS);
    }
    
    for (i=0; i<final_time; i += time_step) {
        //following the half step velocity verlet method described in https://en.wikipedia.org/wiki/Verlet_integration#Velocity_Verlet with an additional acceleration check at the beginning as this makes the system more stable
        // without the extra acceleration calculationthe earth moon sun system visibly drifts within 10 years whereas with the extra calcualtion it remains stable even for 100 years
<<<<<<< HEAD
        for (j=0; j<bodies[0].number_of_bodies; j++) {
            for (k=0; k<NUM_OF_COORDS; k++) {
                bodies[j].acceleration[k] = calculate_acceleration(bodies, j, k);
            }
            for (k=0; k<NUM_OF_COORDS; k++) {
                bodies[j].velocity[k] += (0.5 * bodies[j].acceleration[k] * time_step); //intermediate velocity
=======
        for (j=0; j<body_num; j++) {
            for (k=0; k<NUM_OF_COORDS; k++) {
                bodies[j].acceleration[k] = calculate_acceleration(bodies, j,body_num, k);
            }
            for (k=0; k<NUM_OF_COORDS; k++) {
                bodies[j].velocity[k] += (0.5 * bodies[j].acceleration[k] * time_step); //intermidate velocity
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
            }
            for (k=0; k<NUM_OF_COORDS; k++) {
                bodies[j].position[k] += bodies[j].velocity[k]*time_step; //new position
                fprintf(output, "%lg\t", bodies[j].position[k]);
                fprintf(output, "\t");
            }
            for (k=0; k<NUM_OF_COORDS; k++) {
<<<<<<< HEAD
                bodies[j].acceleration[k] = calculate_acceleration(bodies, j, k);
=======
                bodies[j].acceleration[k] = calculate_acceleration(bodies, j,body_num, k);
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
            }
            for (k=0; k<NUM_OF_COORDS; k++) {
                bodies[j].velocity[k] += 0.5*bodies[j].acceleration[k]*time_step; //new velocity
            }
<<<<<<< HEAD
                get_total_energy(i, bodies, energy_output);
=======
                get_total_energy(i, bodies, body_num, energy_output);
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
        }
        fprintf(output, "\n");
    }
    return NO_ERROR;
}

<<<<<<< HEAD
static double calculate_acceleration(Body *bodies, unsigned  int current_body, Coords dimension){
    unsigned int i,j;
    double accel = 0.0;
    double distance = 0.0;
    for (i=0; i<bodies[0].number_of_bodies; i++) {
=======
static double calculate_acceleration(Body * bodies, unsigned  int current_body, unsigned  int total_bodies, Coords dimension){
    //fprintf(output, "%lg\t%lg\t%lg\t\n\n", bodies[0].acceleration[X], bodies[0].acceleration[Y], bodies[0].acceleration[Z]);
    total_bodies = 3;
    unsigned int i,j;
    double accel = 0.0;
    double distance = 0.0;
    for (i=0; i<total_bodies; i++) {
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
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
<<<<<<< HEAD
static long get_total_energy(unsigned long current_time, Body *bodies, FILE * energy_output){
    unsigned int j,k;
=======
static long get_total_energy(unsigned long current_time, Body * bodies,unsigned int number_of_bodies, FILE * energy_output){
    unsigned int j,k;
    
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
    double kinetic_energy = 0;
    double potential_energy = 0;
    double total_energy = 0;
    double temp =0;
<<<<<<< HEAD
        kinetic_energy = get_kinetic_energy(bodies, bodies[0].number_of_bodies);
        for (j=0; j<bodies[0].number_of_bodies; j++) {
            for (k=0; k<NUM_OF_COORDS; k++) {
                temp += get_potential_energy(bodies, j);
=======
        kinetic_energy = get_kinetic_energy(bodies, number_of_bodies);
        for (j=0; j<number_of_bodies; j++) {
            for (k=0; k<NUM_OF_COORDS; k++) {
                temp += get_potential_energy(bodies, number_of_bodies, j);
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
            }
            potential_energy += temp;
        }
        //potential_energy = fabs(potential_energy);
        total_energy = kinetic_energy + potential_energy;
        fprintf(energy_output, "%ld\t%lg\t%lg\t%lg\n",current_time, kinetic_energy, potential_energy, total_energy);
    return NO_ERROR;
}

static double get_kinetic_energy(Body * bodies, unsigned int number_of_bodies){
    unsigned int i;
    double kinetic_energy=0;
    double total_velocity=0;
    for (i = 0; i<number_of_bodies; i++) {
        total_velocity = sqrt(pow(bodies[i].velocity[X],2) + pow(bodies[i].velocity[Y], 2) + pow(bodies[i].velocity[Z], 2));
        kinetic_energy += 0.5 * bodies[i].mass * pow(total_velocity, 2);
    }
    return kinetic_energy;
}

<<<<<<< HEAD
static double get_potential_energy(Body *bodies, unsigned int current_body){
    unsigned int i,j;
    double pot_en = 0.0;
    double distance = 0.0;
    for (i=0; i<bodies[0].number_of_bodies; i++) {
=======
static double get_potential_energy(Body * bodies, unsigned int number_of_bodies, unsigned int current_body){
    unsigned int i,j;
    double pot_en = 0.0;
    double distance = 0.0;
    for (i=0; i<number_of_bodies; i++) {
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
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

//------------------------------------------------------------------------------------------------------------------------------------------
//Functions for use in the gsl RK4 implementation
<<<<<<< HEAD


static int function (double t, const double y[], double f[], void * params){
    
    /* set up as follows:
     dv/dt = x --> f[0] = y[3]
     da/dt = v --> f[3] = calculate_accel
     Array structure is as follows. Each bodies parameters is a contiguous region of the array of size 6.
     i.e [x0, y0, z1, ax1, ay1, az1 x2, y2, z2, ax2, ay2, az2]
     NUM_OF_COORDS * number_of_bodies is the offset
     however in this case we need a so have to calculate it manually.*/
    (void)(t);
    unsigned int i,j;
    Body * bodies = (Body*)params;
    unsigned int number_of_bodies = bodies[0].number_of_bodies;
        for (i=0; i<number_of_bodies; i++) {
            for (j=0; j<NUM_OF_COORDS; j++) {
                //setting the next position of each body
                bodies[i].position[j]=y[i*(NUM_OF_COORDS*NUM_OF_PARAMS)+j];
            }
            for (j=0; j<NUM_OF_COORDS; j++) {
                //sets the velocity of the next time step
                f[i*(NUM_OF_PARAMS*NUM_OF_COORDS)+j] = y[i*(NUM_OF_PARAMS*NUM_OF_COORDS)+j+NUM_OF_COORDS];
            }
            for (j=0; j<NUM_OF_COORDS; j++) {
                //calculates and sets the acceleration of the body
                f[i*(NUM_OF_COORDS*NUM_OF_PARAMS)+j+NUM_OF_COORDS] = calculate_acceleration(bodies, i, j);
                bodies[i].acceleration[j] = f[i*(NUM_OF_COORDS*NUM_OF_PARAMS)+j+NUM_OF_COORDS];
            }
        }
    return GSL_SUCCESS;
=======
 int function (double t, const double y[], double f[], struct param_type * my_params){
    unsigned int number_of_bodies = 3;
     (void)(t);
     unsigned int i,j,k;
     for (i=0; i<number_of_bodies; i++) {
        for (j=0; j<NUM_OF_PARAMS; j++) {
            for (k=0; k<NUM_OF_COORDS; k++) {
                if (j==0) {
                    f[three_d_to_one_d_index(number_of_bodies, i, j, k)] = y[three_d_to_one_d_index(number_of_bodies, i, j, k)];
                }
                if (j==1) {
                    /*double accel = 0.0;
                    double distance = 0.0;
                    for (l=0; l<number_of_bodies; l++) {
                        if (i != l) {
                            distance=0.0;
                            for (m=0; m<NUM_OF_COORDS; m++) {
                                distance += pow(bodies[l].position[j]-bodies[i].position[j], 2);
                            }
                            distance = sqrt(distance);
                            accel += -(BIG_G * bodies[i].mass * (bodies[l].position[m]-bodies[i].position[m])) /pow(distance,3);
                        }
                    }*/
                    f[three_d_to_one_d_index(number_of_bodies, i, j, k)]  = calculate_accelerationRK4(my_params, i, k);
                }
            }
        }
    }
    return GSL_SUCCESS;
}

static double calculate_accelerationRK4(struct param_type * my_params, unsigned  int current_body, Coords dimension){
    //fprintf(output, "%lg\t%lg\t%lg\t\n\n", bodies[0].acceleration[X], bodies[0].acceleration[Y], bodies[0].acceleration[Z]);
    unsigned int i,j;
    double accel = 0.0;
    double distance = 0.0;
    /*for (i=0; i<my_params->number_of_bodies; i++) {
        if (i != current_body) {
            distance=0.0;
            for (j=0; j<NUM_OF_COORDS; j++) {
                distance += pow(my_params[current_body].position[j]-my_params[i].position[j], 2);
            }
            distance = sqrt(distance);
            accel += -(BIG_G * my_params[i].mass * (my_params[current_body].position[dimension]-my_params[i].position[dimension])) /pow(distance,3);
        }
    }*/
    return accel;
}

static int three_d_to_one_d_index(unsigned int num_bodies, int i, int j, int k){
    int ret_val;
        ret_val = k+(j*num_bodies)+(i*num_bodies*NUM_OF_PARAMS);
    return ret_val;
>>>>>>> 979cfd9ff7a1a24fb1665ff097d9dade6cff9713
}

