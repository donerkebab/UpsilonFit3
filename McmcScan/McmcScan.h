/* 
 * File:   McmcScan.h
 * Author: donerkebab
 *
 * 
 * 
 * 
 * Dev notes:
 * * I would have liked to use a std::array to hold the chains, but the size of
 *   the array is not known at compile-time.
 * 
 * Created on March 19, 2014, 11:19 PM
 */

#ifndef MCMC_MCMCSCAN_H
#define	MCMC_MCMCSCAN_H

#include <string>
#include <utility>
#include <vector>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_vector.h>

#include "MarkovChain.h"

namespace Mcmc {

    class McmcScan {
    public:
        McmcScan(unsigned int max_steps,
                double burn_fraction,
                unsigned int num_chains,
                unsigned int buffer_size,
                std::string output_folder_path,
                unsigned int tries_per_direction,
                std::pair<double, double> init_likelihood_bounds,
                gsl_vector const* benchmark_point);
        virtual ~McmcScan();



    private:
        McmcScan(McmcScan const& orig);
        void operator=(McmcScan const& orig);

        /*
         * Initializes the chains randomly around the benchmark point.
         */
        void InitializeChains(unsigned int num_chains,
                unsigned int buffer_size,
                std::string output_folder_path,
                unsigned int tries_per_direction,
                std::pair<double, double> init_likelihood_bounds,
                gsl_vector const* benchmark_point);

        /*
         * Initializes the vector mean of the parameters of each chain's last 
         * point, the covariance matrix, inverse covariance matrix, and
         * determinant of the covariance matrix.
         */
        void InitializeLastPointsMeanAndCovariance();

        /*
         * Constructs a trial point from the last point in the chain to update.
         */
        std::shared_ptr<Mcmc::Point> TrialPoint(unsigned int chain_to_update);

        virtual bool IsValidParameters(gsl_vector const* parameters) = 0;
        virtual void Measure() = 0;



        std::vector<Mcmc::MarkovChain> chains_;

        unsigned int const max_steps_;
        double const burn_fraction_;
        gsl_rng* rng_;

        unsigned int num_steps_;
        gsl_vector* last_points_mean_;
        gsl_matrix* last_points_covariance_;
        gsl_matrix* last_points_covariance_inv_;
        double last_points_covariance_det_;
    };

}

#endif	/* MCMC_MCMCSCAN_H */

