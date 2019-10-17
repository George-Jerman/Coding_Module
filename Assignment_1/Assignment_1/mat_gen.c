/*
 Title:   Random Matrix Generator
 Author:  Charles D. H. Williams
 Address: Dept of Physics, University of Exeter, UK
 Licence: Public Domain
 */

static const char * VERSION  = "1.0.2";
static const char * REV_DATE = "15-Oct-2019";

/*
 Date         Version  Comments
 ----         -------  --------
 15-Oct-2019    1.0.2  Add seed facility and Gaussian option to RNG
 14-Oct-2019    1.0.1  Initial release.
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h> /* for parsing command line */
#include <math.h>   /* for the Box-Muller method */

/*
 This code, 'mat_gen.c' for a simple program that writes a random matrix,
 specified by the command line arguments, to the stdout file descriptor.
 The ASCII tab character delimits columns, and the newline character
 separates rows. For example, invoking it in this manner:
 
 ./mat_gen  --rows 3 --cols 4 --max 10.0 --min 0.0 --file matrix_1.txt
 
 will create a new file ‘matrix_1.txt’ in the current directory containing
 
 # ./mat_gen --rows 3 --cols 4 --max 10.0 --min 0.0 --file matrix_1.txt
 # Version = 1.0.2, Revision date = 15-Oct-2019
 matrix 3 4
 <DATA>
 end
 
 where lines starting with the ‘#’ character are descriptive comments. The
 line ‘matrix 3 4’ is present to allow memory for the matrix to be allocated
 in one simple step. The ‘<DATA>’ is three lines of four tab-separated random
 values distributed uniformly in the interval [0,10], which comprise matrix_1.
 
 Omitting the '--file' specification will cause the output to be written to
 stdout.
 
 The '--normal' flag wil generate random elements from a normal (Gaussian)
 distribution of mean 0.0 and variance 1.0
 
 The '--seed N' specification, will use the integer N to seed the random()
 function instead of using the default time-based seeding.
 
 */

#define RANDOM_MAX   0x7FFFFFFF /* Maximum value returned by random(3) */

/* Constants defining default behaviour */

static const double DEFAULT_MIN = 0.0;
static const double DEFAULT_MAX = 1.0;

/* Useful definitions */

#define NO  0
#define YES 1

/* Flags defining behaviour within this file. */

static int verbose_flg = 0; /* Just an example, not in use. */

/* Constants for signalling errors: */

typedef enum {
    NO_ERROR = 0,
    NO_MEMORY = 1,
    BAD_ARGS = 2,
    BAD_FILENAME = 3,
    UNKNOWN_ERROR = -1
} Error;

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

/* Read an argument of type 'double' */
static Error get_double_arg( double *value, const char *opt_name, char *optarg) {
    char * endptr = NULL;
    *value = strtod(optarg, &endptr);
    if (*endptr) {
        printf ("Error: option -%s has an invalid argument `%s'.\n", opt_name, optarg);
        return BAD_ARGS;
    }
    return NO_ERROR;
}

/* Read an argument of type 'string' */
static Error get_string_arg( char **value, const char *opt_name, char *optarg) {
    *value = optarg;
    if (*value == NULL) {
        printf ("Error: option -%s has an invalid argument NULL.\n", opt_name);
        return BAD_ARGS;
    }
    return NO_ERROR;
}

/* Generate a pseudo random variate from U[min,max] */
static double uniform(double min, double max) {
    return (random() / (double)RANDOM_MAX) * (max - min) + min ;
}

/* Generate a pseudo random Gaussian variate from N(0,1) */
static double gaussian(void) {
    /* Variables that retain their values between calls: */
    static long counter = 0;
    static double spare_value;
    
    /*
     The Box-Muller method used here generates two variates so we stash
     the second in 'spare_value' and return it next time we are called.
     */
    if (++counter & 1) {
        /* counter is odd */
        double u1 = uniform(0.0, 1.0);
        double u2 = uniform(0.0, 1.0);
        spare_value = sqrt(-log(u1))*cos(2.0*M_PI*u2);
        return        sqrt(-log(u1))*sin(2.0*M_PI*u2);
    }
    /* counter is even */
    return spare_value;
}

/* Generate a random matrix and print it to 'outfile' */
static Error print_matrix( FILE * outfile,
                          long rows, long cols,   /* matrix dimensions */
                          double min, double max, /* range for uniform RNG */
                          int normal_flg,         /* generate a Gaussian distribution? */
                          long seed )             /* RNG seed */
{
    if (( rows < 1 ) || ( cols < 1 )) {
        fprintf(stderr, "Error: 'rows' and 'cols' values are missing or invalid.\n" );
        return BAD_ARGS;
    }
    if (normal_flg == NO && !( min < max )) {
        /* check max and min if a uniform distribution is being used */
        fprintf(stderr, "Error: Value of 'max' is not greater than 'min'.\n" );
        return BAD_ARGS;
    }
    if (seed) {
        /* A non-zero seed was specified as a command line argument */
        srandom((unsigned)seed);
    } else {
        /* Use system-provided random seed */
        srandomdev();
    }
    
    fprintf( outfile, "matrix %ld %ld\n", rows, cols );
    for ( int i = 0; i < rows; i++ ) {
        for ( int j = 0; j < cols; j++ ) {
            double element_ij = (normal_flg) ? gaussian() : uniform(max, min);
            fprintf( outfile, "%.12g\t", element_ij );
        }
        fprintf( outfile, "\n" );
    }
    fprintf( outfile, "end\n" );
    
    return NO_ERROR;
}


/*
 The main() function parses the command line and invokes the function
 that does the work needed to generate and print the random matrix.
 */

int main(int argc,char ** argv)
{
    Error ret_val = NO_ERROR;
    long rows = 0, cols = 0;
    double max = DEFAULT_MAX, min = DEFAULT_MIN;
    char * output_fname = NULL;
    FILE * output_fd = stdout;
    static int normal_flg = NO;
    long seed = 0;
    
    while (1) {
        static struct option long_options[] = {
            /* These options set flags. */
            {"verbose", no_argument,      &verbose_flg, 1},
            {"normal", no_argument,       &normal_flg, 1},
            /* These options don’t set a flag the are edistinguished by their indices. */
            {"rows",  required_argument,  0, 'r'},
            {"cols",  required_argument,  0, 'c'},
            {"max",   required_argument,  0, 'H'},
            {"min",   required_argument,  0, 'L'},
            {"file",  required_argument,  0, 'f'},
            {"seed",  required_argument,  0, 's'},
            {0, 0, 0, 0}
        };
        
        /* getopt_long needs somewhere to store its option index. */
        int option_index = 0;
        
        int c = getopt_long( argc, argv, ":vr:c:H:L:f:s:", long_options, &option_index );
        
        /* End of options is signalled with '-1' */
        if (c == -1)
            break;
        
        switch (c) {
            case 0:
                /* If this option sets a flag we have nothing to do. */
                if (long_options[option_index].flag != 0) {
                    break;
                }
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;
            case 'f':
                ret_val = get_string_arg( &output_fname, long_options[option_index].name, optarg);
                break;
            case 'r':
                ret_val = get_long_arg( &rows, long_options[option_index].name, optarg);
                break;
            case 'c':
                ret_val = get_long_arg( &cols, long_options[option_index].name, optarg);
                break;
            case 's':
                ret_val = get_long_arg( &seed, long_options[option_index].name, optarg);
                break;
            case 'H':
                ret_val = get_double_arg( &max, long_options[option_index].name, optarg);
                break;
            case 'L':
                ret_val = get_double_arg( &min, long_options[option_index].name, optarg);
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
    }
    
    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        fprintf (stderr, "Error: Unrecognised arguments: ");
        while (optind < argc) {
            fprintf (stderr, "%s ", argv[optind++]);
        }
        fprintf (stderr, "\n");
        ret_val = BAD_ARGS;
    }
    if (ret_val != NO_ERROR)
        goto bail_out;
    
    if (output_fname) {
        output_fd = fopen(output_fname,"w");
        if (!output_fd) {
            fprintf (stderr, "Error: Unable to open file '%s'\n", output_fname);
            ret_val = BAD_FILENAME;
        }
    }
    if (ret_val != NO_ERROR)
        goto bail_out;
    
    fprintf( output_fd, "# ");
    for ( int arg_no = 0; arg_no  <argc; arg_no++ ) {
        fprintf(output_fd, "%s ", argv[arg_no]);
    }
    fprintf( output_fd, "\n");
    fprintf( output_fd, "# Version = %s, Revision date = %s\n", VERSION, REV_DATE);
    ret_val = print_matrix(output_fd, rows, cols, min, max, normal_flg, seed);
    
bail_out:
    fclose(output_fd);
    return ret_val;
}
