/* 
 * File:   ToyScan2.h
 * Author: donerkebab
 *
 * Implements a toy Markov chain Monte Carlo scan in a 2-dimensional parameter
 * space, using the Mcmc package.  The scan seeks out a circle around a point in
 * the parameter space.
 * 
 * This class serves as both a sort of test of the Mcmc package, and as a simple
 * example of its use.  
 * 
 * In addition to the normal scan parameters, the user provides a 2D target 
 * point in space, a radius measurement to match to, and an uncertainty in that
 * radius.  The scan should result in a 2D ring around the target point, which 
 * should be good for visualizing the chains moving in.
 * 
 * Created on March 30, 2014, 12:43 PM
 */

#ifndef TOYSCANS_TOYSCAN2_H
#define	TOYSCANS_TOYSCAN2_H

#include <string>
#include <utility>
#include <vector>

#include <gsl/gsl_vector.h>

#include "McmcScan.h"

namespace ToyScans {

    class ToyScan2 : public Mcmc::McmcScan {
    public:
        ToyScan2(unsigned int num_chains,
                unsigned int max_steps,
                double burn_fraction,
                gsl_vector const* center_point,
                double radius,
                double uncertainty);
        virtual ~ToyScan2();

        /*
         * Generates chain seed parameters and filenames.  It just chooses
         * random points for the seeds.  Filenames are generated trivially.
         */
        std::vector<std::pair<gsl_vector*, std::string> > GenerateChainSeeds(
                unsigned int num_chains);

    private:
        ToyScan2(ToyScan2 const& orig);
        void operator=(ToyScan2 const& orig);

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
         * For this scan, the likelihood is a Gaussian function of the radius
         * from the center point, with mean radius_ and uncertainty 
         * uncertainty_.
         * 
         * For this scan, for the measurements, we store the radius and the 
         * theta angle (in radians).
         */
        void MeasurePoint(gsl_vector const* parameters,
                gsl_vector*& measurements,
                double& likelihood);

        gsl_vector* center_point_;
        double radius_;
        double uncertainty_;

    };

}

#endif	/* TOYSCANS_TOYSCAN2_H */

