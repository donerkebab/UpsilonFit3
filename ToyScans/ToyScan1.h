/* 
 * File:   ToyScan1.h
 * Author: donerkebab
 *
 * Implements a toy Markov chain Monte Carlo scan in a 3-dimensional parameter
 * space, using the Mcmc package.  The scan seeks out a single target point in 
 * the parameter space.
 * 
 * This class serves as both a sort of test of the Mcmc package, and as a simple
 * example of its use.
 * 
 * In addition to the normal scan parameters, the user provides a 3D target 
 * point in space to scan for, and uncertainties in the three parameters.  The 
 * scan should result in a 3D Gaussian posterior distribution centered at the
 * target point.
 * 
 * Created on March 28, 2014, 8:45 AM
 */

#ifndef TOYSCANS_TOYSCAN1_H
#define	TOYSCANS_TOYSCAN1_H

#include <string>
#include <utility>
#include <vector>

#include <gsl/gsl_vector.h>

#include "McmcScan.h"

namespace ToyScans {

    class ToyScan1 : public Mcmc::McmcScan {
    public:
        ToyScan1(unsigned int num_chains,
                unsigned int max_steps,
                double burn_fraction,
                gsl_vector const* target_point,
                gsl_vector const* uncertainties);
        virtual ~ToyScan1();

        /*
         * Generates chain seed parameters and filenames.  It just chooses
         * random points for the seeds.  Filenames are generated trivially.
         */
        std::vector<std::pair<gsl_vector*, std::string> > GenerateChainSeeds(
                unsigned int num_chains);
        
    private:
        ToyScan1(ToyScan1 const& orig);
        void operator=(ToyScan1 const& orig);

        /*
         * Determines if the parameters are valid in the parameter space.
         * 
         * For this scan, just for kicks, we keep the chains in a box 
         * [-10, 10] in each dimension.
         */
        bool IsValidParameters(gsl_vector const* parameters);

        /*
         * Calculates the measurements and likelihood for a given set of
         * parameters.  Stores them in output arguments.  GSL vector 
         * measurements gets newly allocated within the method.
         * 
         * Input: parameters
         * Outputs: measurements, likelihood
         * 
         * For this scan, the likelihood is a Gaussian function of the distance
         * between the parameters and the target point, with mean 0 and the
         * user-supplied uncertainties.
         * 
         * For this scan, for the measurements, we store the magnitude and the
         * theta and phi angles (in radians) of the vector from the target point
         * to the chain's current point.
         */
        void MeasurePoint(gsl_vector const* parameters,
                gsl_vector*& measurements,
                double& likelihood);

        gsl_vector* target_point_;
        gsl_vector* uncertainties_;
    };

}

#endif	/* TOYSCANS_TOYSCAN1_H */

