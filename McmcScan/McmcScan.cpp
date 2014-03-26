/* 
 * File:   McmcScan.cpp
 * Author: donerkebab
 * 
 * Created on March 19, 2014, 11:19 PM
 */

#include "McmcScan.h"

#include <cmath>
#include <ctime>

#include <array>
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

#include "ChainFlushError.h"
#include "MarkovChain.h"
#include "PositiveDefiniteError.h"

namespace Mcmc {

    McmcScan::McmcScan(unsigned int dimension, unsigned int num_chains,
            unsigned int max_steps, double burn_fraction)
    : dimension_(dimension),
    num_chains_(num_chains),
    max_steps_(max_steps),
    burn_fraction_(burn_fraction),
    num_steps_(0) {
        if (dimension == 0 || num_chains == 0 || max_steps == 0 ||
                burn_fraction < 0.0 || burn_fraction > 1.0) {
            throw std::invalid_argument("invalid input to McmcScan");
        }

        // Sanity check: require more chains than dimensions, or else the
        // covariance matrix will be singular
        if (num_chains_ <= dimension_) {
            throw std::invalid_argument("need more chains than dimensions");
        }

        // Initialize the random number generator
        rng_ = gsl_rng_alloc(gsl_rng_default);
        gsl_rng_set(rng_, std::time(nullptr));

        // Initialize the chains
        InitializeChains();

        // Sanity check: require that we have the proper number of chains
        if (chains_.size() != num_chains_) {
            throw std::invalid_argument("wrong number of chains");
        }

        // Sanity check: require that each chain is seeded with a point of the
        // proper dimension
        for (std::vector<Mcmc::MarkovChain>::const_iterator i_chain =
                chains_.begin(); i_chain < chains_.end(); ++i_chain) {
            if (i_chain->last_point()->parameters()->size != dimension_) {
                throw std::invalid_argument("chain seed has wrong dimension");
            }
        }

        // Initialize the last points' mean, covariance
        InitializeLastPointsMeanAndCovariance();
    }

    McmcScan::~McmcScan() {
        // The chains are automatically fully flushed when destroyed.
        gsl_rng_free(rng_);
        gsl_vector_free(last_points_mean_);
        gsl_matrix_free(last_points_covariance_);
        gsl_matrix_free(last_points_covariance_inv_);
    }

    void McmcScan::Run() {
        while (num_steps_ < max_steps_) {
            // Increment num_steps_ here to get the right value for Lambda()
            ++num_steps_;

            // Randomly choose a chain to update
            unsigned int chain_to_update = gsl_rng_uniform_int(rng_,
                    num_chains_);
            std::shared_ptr<Mcmc::Point> last_point = 
                    chains_[chain_to_update].last_point();

            // Construct a trial point and compute the trial mean and covariance
            std::shared_ptr<Mcmc::Point> trial_point = TrialPoint(last_point);
            gsl_vector* trial_mean;
            gsl_matrix* trial_covariance;
            double trial_covariance_det;
            gsl_matrix* trial_covariance_inv;
            TrialMeanAndCovariance(last_point, trial_point, trial_mean,
                    trial_covariance, trial_covariance_det,
                    trial_covariance_inv);

            // Compute the acceptance ratio and decide
            double acceptance_ratio = AcceptanceRatio(last_point, trial_point,
                    trial_covariance_det, trial_covariance_inv);

            if (gsl_rng_uniform(rng_) <= acceptance_ratio) {
                gsl_vector_free(last_points_mean_);
                gsl_matrix_free(last_points_covariance_);
                gsl_matrix_free(last_points_covariance_inv_);

                try {
                    chains_[chain_to_update].Append(trial_point);
                } catch (Mcmc::ChainFlushError& e) {
                    std::printf("Error flushing chain %u, will try again next time",
                            chain_to_update);
                }
                last_points_mean_ = trial_mean;
                last_points_covariance_ = trial_covariance;
                last_points_covariance_det_ = trial_covariance_det;
                last_points_covariance_inv_ = trial_covariance_inv;
            } else {
                gsl_vector_free(trial_mean);
                gsl_matrix_free(trial_covariance);
                gsl_matrix_free(trial_covariance_inv);

                try {
                    chains_[chain_to_update].Append(last_point);
                } catch (Mcmc::ChainFlushError& e) {
                    std::printf("Error flushing chain %u, will try again next time",
                            chain_to_update);
                }
            }
        }
    }

    void McmcScan::InitializeLastPointsMeanAndCovariance() {
        // Compute the vector mean
        last_points_mean_ = gsl_vector_calloc(dimension_);
        for (std::vector<Mcmc::MarkovChain>::const_iterator i_chain =
                chains_.begin();
                i_chain < chains_.end(); ++i_chain) {
            gsl_vector_add(last_points_mean_,
                    i_chain->last_point()->parameters());
        }
        gsl_vector_scale(last_points_mean_, 1.0 / num_chains_);

        // Compute the covariance matrix
        // gsl_blas_dger: "rank-1 update (4') = (1)(2)(3)^T + (4)"
        last_points_covariance_ = gsl_matrix_calloc(dimension_, dimension_);
        gsl_vector* temp = gsl_vector_alloc(dimension_);
        for (std::vector<Mcmc::MarkovChain>::const_iterator i_chain =
                chains_.begin();
                i_chain < chains_.end(); ++i_chain) {
            gsl_vector_memcpy(temp, i_chain->last_point()->parameters());
            gsl_vector_sub(temp, last_points_mean_);
            gsl_blas_dger(1.0 / num_chains_, temp, temp,
                    last_points_covariance_);
        }
        gsl_vector_free(temp);

        // Compute the intermediates for the determinant and inverse of the 
        // covariance matrix
        gsl_matrix* covariance_lu = gsl_matrix_alloc(dimension_, dimension_);
        gsl_matrix_memcpy(covariance_lu, last_points_covariance_);
        gsl_permutation* covariance_p = gsl_permutation_alloc(dimension_);
        int covariance_signum;
        gsl_linalg_LU_decomp(covariance_lu, covariance_p, &covariance_signum);

        // Compute the determinant of the covariance matrix
        last_points_covariance_det_ = gsl_linalg_LU_det(covariance_lu,
                covariance_signum);

        // Sanity check: covariance matrix is nonsingular and positive definite
        if (last_points_covariance_det_ <= 0.0) {
            throw new Mcmc::PositiveDefiniteError();
        }

        // Compute the inverse of the covariance matrix
        last_points_covariance_inv_ = gsl_matrix_alloc(dimension_, dimension_);
        gsl_linalg_LU_invert(covariance_lu, covariance_p,
                last_points_covariance_inv_);

        // Free memory for intermediates
        gsl_matrix_free(covariance_lu);
        gsl_permutation_free(covariance_p);
    }

    std::shared_ptr<Mcmc::Point> McmcScan::TrialPoint(
            std::shared_ptr<Mcmc::Point> last_point) {
        double f = 2.381 / std::sqrt(dimension_);

        // Compute the Cholesky decomposition of the covariance matrix
        // gsl_linalg_cholesky_decomp: Cholesky decomposition of symmetric,
        // positive-definite, square argument, only requires lower triangle.
        // However, this returns L in the lower triangle and L^T overwritten
        // in the upper triangle.
        gsl_matrix* last_covariance_cholesky = gsl_matrix_alloc(dimension_,
                dimension_);
        gsl_matrix_memcpy(last_covariance_cholesky, last_points_covariance_);
        gsl_linalg_cholesky_decomp(last_covariance_cholesky);

        // Keep generating trial points until we get one with valid parameters
        gsl_vector* trial_parameters = gsl_vector_alloc(dimension_);
        while (true) {
            // Construct a vector of random components from a unit Gaussian
            for (int i = 0; i < dimension_; ++i) {
                gsl_vector_set(trial_parameters, i, gsl_ran_ugaussian(rng_));
            }

            // Scale the vector with f*L to get the trial shift, where L is
            // the Cholesky decomposition of the covariance matrix.
            // gsl_blas_dtrmv: "matrix-vector product for the triangular matrix
            // (5') = (4)(5)"
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

    void McmcScan::TrialMeanAndCovariance(
            std::shared_ptr<Mcmc::Point> last_point,
            std::shared_ptr<Mcmc::Point> trial_point,
            gsl_vector* trial_mean, gsl_matrix* trial_covariance,
            double& trial_covariance_det, gsl_matrix* trial_covariance_inv) {
        // Calculate the trial shift
        // trial_shift = trial_parameters - last_parameters
        gsl_vector* trial_shift = gsl_vector_alloc(dimension_);
        gsl_vector_memcpy(trial_shift, trial_point->parameters());
        gsl_vector_sub(trial_shift, last_point->parameters());

        // Update the vector mean
        // mean' = mean + trial_shift/num_chains
        // gsl_blas_daxpy: "sum (3') = (1)(2) + (3)"
        trial_mean = gsl_vector_alloc(dimension_);
        gsl_vector_memcpy(trial_mean, last_points_mean_);
        gsl_blas_daxpy(1.0 / num_chains_, trial_shift, trial_mean);

        // Get intermediates for the calculation of covariance matrix quantities
        std::array<gsl_vector*, 2> a;
        std::array<gsl_vector*, 2> b;
        for (int i = 0; i < 2; ++i) {
            a[0] = gsl_vector_alloc(dimension_);
            b[0] = gsl_vector_alloc(dimension_);
        }
        // a[0] = trial_shift/num_chains
        gsl_vector_memcpy(a[0], trial_shift);
        gsl_vector_scale(a[0], 1.0 / num_chains_);
        // b[0] = last_parameters - last_mean
        gsl_vector_memcpy(b[0], last_point->parameters());
        gsl_vector_sub(b[0], last_points_mean_);
        // a[1] = 1/(num_chains) * (last_parameters - last_mean + 
        //        (num_chains - 1)/num_chains * trial_shift)
        // gsl_blas_daxpy: "sum (3') = (1)(2) + (3)"
        gsl_vector_memcpy(a[1], last_point->parameters());
        gsl_vector_sub(a[1], last_points_mean_);
        gsl_blas_daxpy((num_chains_ - 1.0) / num_chains_, trial_shift, a[1]);
        gsl_vector_scale(a[1], 1.0 / num_chains_);
        // b[1] = trial_shift
        gsl_vector_memcpy(b[1], trial_shift);
        // one_plus_lambda[i,j] = I[i,j] + b[i]^T C^-1 a[j]
        // gsl_blas_dgemv: "the matrix-vector product and sum
        //                  (6') = (2)(3)(4) + (5)(6)"
        // gsl_blas_ddot: "the scalar product (3) = (1)^T (2)"
        gsl_matrix* one_plus_lambda = gsl_matrix_calloc(2, 2);
        double temp_double;
        gsl_vector* temp_vector = gsl_vector_alloc(dimension_);
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                gsl_vector_set_zero(temp_vector);
                gsl_blas_dgemv(CblasNoTrans, 1.0, last_points_covariance_inv_,
                        a[j], 0.0, temp_vector);
                gsl_blas_ddot(b[i], temp_vector, &temp_double);
                if (i == j) {
                    temp_double += 1.0;
                }
                gsl_matrix_set(one_plus_lambda, i, j, temp_double);
            }
        }
        gsl_vector_free(temp_vector);
        // Determinant and inverse of one_plus_lambda
        // Using the GSL linear algebra library requires computing the LU
        //   decomposition, and is too heavy-handed for this application.
        // Note that if the determinant of one_plus_lambda is zero, then the
        //   resulting trial covariance matrix will also have determinant zero.
        //   This occurs almost never, but in case it does, we might as well 
        //   check for it here before it is used to compute the inverse.
        double one_plus_lambda_det = gsl_matrix_get(one_plus_lambda, 0, 0) *
                gsl_matrix_get(one_plus_lambda, 1, 1) -
                gsl_matrix_get(one_plus_lambda, 0, 1) *
                gsl_matrix_get(one_plus_lambda, 1, 0);
        if (one_plus_lambda_det <= 0.0) {
            // Free memory of intermediates
            gsl_vector_free(trial_shift);
            for (int i = 0; i < 2; ++i) {
                gsl_vector_free(a[i]);
                gsl_vector_free(b[i]);
            }
            gsl_matrix_free(one_plus_lambda);

            throw new Mcmc::PositiveDefiniteError();
        }
        gsl_matrix* one_plus_lambda_inv = gsl_matrix_alloc(2, 2);
        gsl_matrix_set(one_plus_lambda_inv, 0, 0,
                gsl_matrix_get(one_plus_lambda, 1, 1) / one_plus_lambda_det);
        gsl_matrix_set(one_plus_lambda_inv, 0, 1,
                -1.0 * gsl_matrix_get(one_plus_lambda, 0, 1) /
                one_plus_lambda_det);
        gsl_matrix_set(one_plus_lambda_inv, 1, 0,
                -1.0 * gsl_matrix_get(one_plus_lambda, 1, 0) /
                one_plus_lambda_det);
        gsl_matrix_set(one_plus_lambda_inv, 1, 1,
                gsl_matrix_get(one_plus_lambda, 0, 0) / one_plus_lambda_det);



        // Update the covariance matrix
        // C' = C + a[0]*b0^T + a[1]*b[1]^T
        // gsl_blas_dger: "the rank-1 update (4') = (1)(2)(3)^T + (4)"
        trial_covariance = gsl_matrix_alloc(dimension_, dimension_);
        gsl_matrix_memcpy(trial_covariance, last_points_covariance_);
        for (int i = 0; i < 2; ++i) {
            gsl_blas_dger(1.0, a[i], b[i], trial_covariance);
        }

        // Update the determinant of the covariance matrix
        // det(C') = det(C) * det(one_plus_lambda)
        trial_covariance_det = last_points_covariance_det_ *
                one_plus_lambda_det;

        // Update the inverse of the covariance matrix
        // C'^-1 = C^-1 - 
        //         sum_i sum_j (one_plus_lambda^-1)[i][j] C^-1 a[i] b[j]^T C^-1
        // gsl_blas_dger: "the rank-1 update (4') = (1)(2)(3)^T + (4)"
        // gsl_blas_dgemm: "the matrix-matrix product and sum
        //                  (7') = (3)(4)(5) + (6)(7)"
        trial_covariance_inv = gsl_matrix_alloc(dimension_, dimension_);
        gsl_matrix* temp_matrix1 = gsl_matrix_alloc(dimension_, dimension_);
        gsl_matrix* temp_matrix2 = gsl_matrix_alloc(dimension_, dimension_);
        gsl_matrix_memcpy(trial_covariance_inv, last_points_covariance_inv_);
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                gsl_matrix_set_zero(temp_matrix1);
                gsl_matrix_set_zero(temp_matrix2);
                gsl_blas_dger(1.0, a[i], b[j], temp_matrix1);
                gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, temp_matrix1,
                        last_points_covariance_inv_, 0.0, temp_matrix2);
                gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
                        gsl_matrix_get(one_plus_lambda_inv, i, j),
                        last_points_covariance_inv_, temp_matrix2, 0.0,
                        temp_matrix1);
                gsl_matrix_sub(trial_covariance_inv, temp_matrix1);
            }
        }
        gsl_matrix_free(temp_matrix1);
        gsl_matrix_free(temp_matrix2);

        // Free memory of intermediates
        gsl_vector_free(trial_shift);
        for (int i = 0; i < 2; ++i) {
            gsl_vector_free(a[i]);
            gsl_vector_free(b[i]);
        }
        gsl_matrix_free(one_plus_lambda);
        gsl_matrix_free(one_plus_lambda_inv);
    }

    double McmcScan::Lambda() {
        double lambda = 1.0;

        if (num_steps_ <= burn_fraction_ * max_steps_ / 2.0) {
            lambda = std::pow(0.01,
                    1.0 - num_steps_ / (burn_fraction_ * max_steps_ / 2.0));
        }

        return lambda;
    }

    double McmcScan::AcceptanceRatio(std::shared_ptr<Mcmc::Point> last_point,
            std::shared_ptr<Mcmc::Point> trial_point,
            double trial_covariance_det,
            gsl_matrix const* trial_covariance_inv) {
        double f = 2.381 / std::sqrt(dimension_);

        // Calculate the trial shift
        // trial_shift = trial_parameters - last_parameters
        gsl_vector* trial_shift = gsl_vector_alloc(dimension_);
        gsl_vector_memcpy(trial_shift, trial_point->parameters());
        gsl_vector_sub(trial_shift, last_point->parameters());

        // Calculate the linear algebra part of the formula
        // gsl_blas_dgemv: "the matrix-vector product and sum
        //                  (6') = (2)(3)(4) + (5)(6)"
        // gsl_blas_ddot: "the scalar product (3) = (1)^T (2)"
        double linear_algebra_part;
        gsl_matrix* temp_matrix = gsl_matrix_alloc(dimension_, dimension_);
        gsl_vector* temp_vector = gsl_vector_alloc(dimension_);
        gsl_matrix_memcpy(temp_matrix, trial_covariance_inv);
        gsl_matrix_sub(temp_matrix, last_points_covariance_inv_);
        gsl_blas_dgemv(CblasNoTrans, 1.0, temp_matrix, trial_shift, 0.0,
                temp_vector);
        gsl_blas_ddot(trial_shift, temp_vector, &linear_algebra_part);
        gsl_matrix_free(temp_matrix);
        gsl_vector_free(temp_vector);
        gsl_vector_free(trial_shift);

        // Calculate the acceptance ratio
        // If either matrix determinant were zero, it would have thrown an 
        // exception already.
        double acceptance_ratio;
        if (last_point->likelihood() == 0.0) {
            acceptance_ratio = 1.0;
        } else {
            acceptance_ratio =
                    std::sqrt(last_points_covariance_det_ / trial_covariance_det) *
                    std::exp(-1.0 / (2.0 * f * f) * linear_algebra_part) *
                    std::pow(trial_point->likelihood() / last_point->likelihood(),
                    Lambda());
        }
        return acceptance_ratio;
    }



}