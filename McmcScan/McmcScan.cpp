/* 
 * File:   McmcScan.cpp
 * Author: donerkebab
 * 
 * Created on March 19, 2014, 11:19 PM
 */

#include "McmcScan.h"

#include <cmath>
#include <ctime>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_vector.h>

#include "MarkovChain.h"

namespace Mcmc {

    McmcScan::McmcScan(unsigned int max_steps,
            double burn_fraction,
            unsigned int num_chains,
            unsigned int buffer_size,
            std::string output_folder_path,
            unsigned int tries_per_direction,
            std::pair<double, double> init_likelihood_bounds,
            gsl_vector const* benchmark_point)
    : max_steps_(max_steps),
    burn_fraction_(burn_fraction),
    num_steps_(0) {

        if (max_steps == 0 ||
                burn_fraction < 0.0 || burn_fraction > 1.0 ||
                num_chains < 2 ||
                buffer_size == 0 ||
                tries_per_direction == 0 ||
                init_likelihood_bounds.first > init_likelihood_bounds.second ||
                benchmark_point == nullptr) {
            throw std::invalid_argument("invalid input to McmcScan");
        }

        // Initialize the random number generator
        rng_ = gsl_rng_alloc(gsl_rng_default);
        gsl_rng_set(rng_, std::time(nullptr));

        // Initialize the Markov chains
        InitializeChains(num_chains, buffer_size, output_folder_path,
                tries_per_direction, init_likelihood_bounds, benchmark_point);

        // Initialize the last points' mean, covariance
        InitializeLastPointsMeanAndCovariance();
    }

    McmcScan::~McmcScan() {
    }

    void McmcScan::InitializeChains(unsigned int num_chains,
            unsigned int buffer_size,
            std::string output_folder_path,
            unsigned int tries_per_direction,
            std::pair<double, double> init_likelihood_bounds,
            gsl_vector const* benchmark_point) {
        // TODO
    }

    void McmcScan::InitializeLastPointsMeanAndCovariance() {
        unsigned int dimension = chains_[0].last_point()->parameters()->size;

        // Compute the vector mean
        last_points_mean_ = gsl_vector_calloc(dimension);

        for (std::vector<Mcmc::MarkovChain>::const_iterator i_chain =
                chains_.begin();
                i_chain < chains_.end(); ++i_chain) {
            gsl_vector_add(last_points_mean_,
                    i_chain->last_point()->parameters());
        }

        gsl_vector_scale(last_points_mean_, 1.0 / chains_.size());

        // Compute the covariance matrix
        // gsl_blas_dger: "rank-1 update (4') = (1)(2)(3)^T + (4)"
        last_points_covariance_ = gsl_matrix_calloc(dimension, dimension);
        gsl_vector* temp = gsl_vector_alloc(dimension);

        for (std::vector<Mcmc::MarkovChain>::const_iterator i_chain =
                chains_.begin();
                i_chain < chains_.end(); ++i_chain) {
            gsl_vector_memcpy(temp, i_chain->last_point()->parameters());
            gsl_vector_sub(temp, last_points_mean_);

            gsl_blas_dger(1.0 / chains_.size(), temp, temp,
                    last_points_covariance_);
        }

        gsl_vector_free(temp);

        // Compute the inverse and determinant of the covariance matrix
        gsl_matrix* covariance_lu = gsl_matrix_alloc(dimension, dimension);
        gsl_matrix_memcpy(covariance_lu, last_points_covariance_);
        gsl_permutation* covariance_p = gsl_permutation_alloc(dimension);
        int covariance_signum;
        gsl_linalg_LU_decomp(covariance_lu, covariance_p, &covariance_signum);

        last_points_covariance_inv_ = gsl_matrix_alloc(dimension, dimension);
        gsl_linalg_LU_invert(covariance_lu, covariance_p,
                last_points_covariance_inv_);

        last_points_covariance_det_ = gsl_linalg_LU_det(covariance_lu,
                covariance_signum);

        gsl_matrix_free(covariance_lu);
        gsl_permutation_free(covariance_p);
    }

    std::shared_ptr<Mcmc::Point> McmcScan::TrialPoint(
            std::shared_ptr<Mcmc::Point> last_point) {
        unsigned int dimension = last_point->parameters()->size;
        double f = 2.381 / std::sqrt(dimension);

        // Compute the Cholesky decomposition of the covariance matrix
        // gsl_linalg_cholesky_decomp: Cholesky decomposition of symmetric,
        // positive-definite, square argument, only requires lower triangle.
        // However, this returns L in the lower triangle and L^T overwritten
        // in the upper triangle.
        gsl_matrix* last_covariance_cholesky = gsl_matrix_alloc(dimension,
                dimension);
        gsl_matrix_memcpy(last_covariance_cholesky, last_points_covariance_);
        gsl_linalg_cholesky_decomp(last_covariance_cholesky);

        // Keep generating trial points until we get one with valid parameters
        gsl_vector* trial_parameters = gsl_vector_alloc(dimension);
        while (true) {
            // Construct a vector of random components from a unit Gaussian
            for (int i = 0; i < dimension; ++i) {
                gsl_vector_set(trial_parameters, i, gsl_ran_ugaussian(rng_));
            }

            // Scale the vector with f*L to get the trial shift, where L is
            // the Cholesky decomposition of the covariance matrix.
            // gsl_blas_dtrmv: "matrix-vector product for the triangular matrix
            // (5') = (4)(5)", where (1) = CblasLower to only use the lower
            // triangle (L instead of L^T), (2) = CblasNoTrans to have no
            // extra operations done on the matrix, and (3) = CblasNonUnit to
            // use the diagonal of the matrix.
            gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit,
                    last_covariance_cholesky, trial_parameters);
            gsl_vector_scale(trial_parameters, f);

            // Now we have the trial shift from the last point, and we need to
            // generate the trial point itself
            gsl_vector_add(trial_parameters, last_point->parameters());

            // Test to see if the new parameters are valid
            if (IsValidParameters(trial_parameters)) {
                gsl_vector* trial_measurements = nullptr;
                double trial_likelihood = 0.0;
                MeasurePoint(trial_parameters, trial_measurements,
                        trial_likelihood);

                std::shared_ptr<Mcmc::Point> trial_point(
                        new Mcmc::Point(trial_parameters, trial_measurements,
                        trial_likelihood));

                gsl_matrix_free(last_covariance_cholesky);

                return trial_point;
            }
        }
    }


}