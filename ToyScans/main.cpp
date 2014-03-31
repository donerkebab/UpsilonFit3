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
#include "ToyScan2.h"


namespace { // unnamed namespace
    // Forward declarations of scan functions
    void RunScan1();
    void RunScan2();
}

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

    switch (scan_selection) {
        case 1:
            ::RunScan1();
            break;
        case 2:
            ::RunScan2();
            break;
        default:
            printf("scan selected does not exist");
    }

    return 0;
}


namespace {

    void RunScan1() {
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

    void RunScan2() {
        unsigned int num_chains = 10;
        unsigned int buffer_size = 20;
        unsigned int max_steps = 100000;
        double burn_fraction = 0.1;

        gsl_vector* center_point = gsl_vector_alloc(2);
        gsl_vector_set(center_point, 0, 2);
        gsl_vector_set(center_point, 1, 1);

        double radius = 3;
        double uncertainty = 0.3;

        ToyScans::ToyScan2 scan(num_chains, max_steps, burn_fraction,
                center_point, radius, uncertainty);

        scan.Initialize(buffer_size, scan.GenerateChainSeeds(num_chains));

        scan.Run();

        // Memory cleanup
        gsl_vector_free(center_point);
    }




}

