/* 
 * File:   main.cpp
 * Author: donerkebab
 *
 * Created on March 28, 2014, 8:45 AM
 */

#include <cstdlib>
#include <cstdio>

#include <gsl/gsl_vector.h>

#include "ToyScan1.h"

/*
 * Runs the selected toy scan.
 * 
 * Input: number of the toy scan to run.
 * 
 */
int main(int argc, char** argv) {

    if (argc != 2) {
        printf("only one input required");
        exit(1);
    }

    int scan_selection = std::atoi(argv[1]);
    if (scan_selection <= 0 || scan_selection >= 2) {
        printf("scan selected does not exist");
        exit(1);
    }

    if (scan_selection == 1) {
        unsigned int num_chains = 10;
        unsigned int buffer_size = 25;
        unsigned int max_steps = 10000;
        double burn_fraction = 0.1;

        gsl_vector* target_point = gsl_vector_alloc(3);
        gsl_vector_set(target_point, 0, 1);
        gsl_vector_set(target_point, 1, 1);
        gsl_vector_set(target_point, 2, 1);

        gsl_vector* uncertainties = gsl_vector_alloc(3);
        gsl_vector_set(uncertainties, 0, 0.1);
        gsl_vector_set(uncertainties, 1, 0.5);
        gsl_vector_set(uncertainties, 2, 1);

        ToyScans::ToyScan1 scan(num_chains, max_steps, burn_fraction,
                target_point, uncertainties);

        scan.Initialize(buffer_size, scan.GenerateChainSeeds(num_chains));

        scan.Run();

        // Memory cleanup
        gsl_vector_free(target_point);
        gsl_vector_free(uncertainties);
    }

    return 0;
}

