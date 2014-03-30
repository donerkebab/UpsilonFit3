/* 
 * File:   McmcScan.h
 * Author: donerkebab
 *
 * Markov chain Monte Carlo (MCMC) scan of a parameter space.  Abstract class.
 * Users must subclass McmcScan, supplying implementations for the pure virtual 
 * methods IsValidParameters(), MeasurePoint(), and InitializeChains().
 * 
 * McmcScan uses an adaptive Metropolis-Hastings algorithm with simulated
 * annealing.  A Gaussian proposal is used to choose the trial shift.  The
 * algorithm is adaptive in that the size of the shift in any direction is based
 * on the covariance matrix of the chains' last points, which is a measure of
 * how large the posterior probability distribution seems to be at that time.
 * These quantities are continuously updated at each step.
 * 
 * Users must first extend this class to supply
 * * IsValidParameters(), which returns true if the parameters are within the
 *   bounds of the parameter space.
 * * MeasurePoint(), which supplies the measurements and likelihood for a given
 *   point in the parameter space.
 * Users should then initialize an instance of the subclass, and then call
 * Initialize() with the chain initialization info, and then Run().  
 * 
 * The random number generator rng_ is protected so that users may use it in
 * their subclass, say, to initialize chains.
 * 
 * 
 * 
 * 
 * Dev notes:
 * * The chains are held in a vector of pointers to MarkovChain objects, because
 *   std::vector requires the objects to be copy-constructible.
 * * I would have liked to use a std::array to hold the chains, but the size of
 *   the array is supplied by the user and not guaranteed to be known at 
 *   compile time.
 * * When MarkovChain::Append() is called in Run(), Mcmc::ChainFlushError only
 *   results in printing a message to stdout.  Flushing will be tried again the
 *   next time MarkovChain::Append(), MarkovChain::Flush(), or the destructor
 *   is called.
 * * For all the private methods, all output pointers are allocated within the
 *   method.  So the output pointers passed in should not be allocated already.
 * 
 * Created on March 19, 2014, 11:19 PM
 */

#ifndef MCMC_MCMCSCAN_H
#define	MCMC_MCMCSCAN_H

#include <memory>
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
        McmcScan(unsigned int dimension,
                unsigned int num_chains,
                unsigned int max_steps,
                double burn_fraction);
        virtual ~McmcScan();

        /*
         * Initializes the chains and other starting variables.
         * 
         * Must be supplied with the chains' initialization information in the
         * form of pairs of seed parameters and filenames.  The GSL vectors
         * passed in for the seed parameters are defensively copied into the 
         * chains, and must be freed by the user.
         * 
         * throws std::logic_error if called more than once
         * 
         * may throw Mcmc::PositiveDefiniteError if the starting covariance 
         * matrix is not positive definite
         * 
         * may throw Mcmc::ChainFlushError if output files cannot be opened
         */
        void Initialize(unsigned int buffer_size, std::vector<
                std::pair<gsl_vector*, std::string> > chains_info);

        /*
         * Runs the scan to completion unless an exception is thrown.
         * 
         * throws std::logic error if called before chains are initialized
         * 
         * may throw Mcmc::PositiveDefiniteError if the covariance matrix
         * becomes no longer positive definite
         * 
         * may throw Mcmc::ChainFlushError if output files cannot be opened
         */
        void Run();

    protected:
        gsl_rng* rng_;

    private:
        McmcScan(McmcScan const& orig);
        void operator=(McmcScan const& orig);

        /*
         * Initializes the chains with the seed parameters and filenames.
         * The resulting chains are stored in member variable chains_.
         * 
         * throws Mcmc::ChainFlushError if output files cannot be opened
         */
        void InitializeChains(unsigned int buffer_size, std::vector<
                std::pair<gsl_vector*, std::string> > chains_info);

        /*
         * Initializes the vector mean of the parameters of each chain's last 
         * point, the covariance matrix, inverse covariance matrix, and
         * determinant of the covariance matrix.  Results are stored in the
         * class's member variables.
         * 
         * throws Mcmc::PositiveDefiniteError if covariance matrix is not
         * positive definite
         */
        void InitializeLastPointsMeanAndCovariance();

        /*
         * Constructs a trial point from the last point in the chain to update.
         */
        std::shared_ptr<Mcmc::Point> TrialPoint(std::shared_ptr<Mcmc::Point>
                last_point);

        /*
         * Calculates the mean and covariance if the trial point were to be
         * accepted.  Stores them in output arguments, which are also newly
         * allocated within the method.
         * 
         * Done by updating the last_* quantities with the new trial point,
         * without having to recalculate them from scratch.  Follows the 
         * procedure in Baltz, et al. (arXiv:hep-ph/0602187)
         * 
         * Inputs: last_point, trial_point
         * Outputs: trial_mean, trial_covariance, trial_covariance_det,
         *            trial_covariance_inv
         * 
         * throws Mcmc::PositiveDefiniteError if trial covariance matrix is not
         * positive definite
         */
        void TrialMeanAndCovariance(std::shared_ptr<Mcmc::Point> last_point,
                std::shared_ptr<Mcmc::Point> trial_point,
                gsl_vector*& trial_mean,
                gsl_matrix*& trial_covariance,
                double& trial_covariance_det,
                gsl_matrix*& trial_covariance_inv);

        /*
         * Calculates lambda, the annealing exponent.  It takes values other
         * than 1 for the first half of the burn-in period.
         */
        double Lambda();

        /*
         * Calculates the acceptance ratio for the trial point.
         */
        double AcceptanceRatio(std::shared_ptr<Mcmc::Point> last_point,
                std::shared_ptr<Mcmc::Point> trial_point,
                double trial_covariance_det,
                gsl_matrix const* trial_covariance_inv);

        /*
         * Determines if the parameters are valid in the parameter space.
         * (abstract)
         */
        virtual bool IsValidParameters(gsl_vector const* parameters) = 0;

        /*
         * Calculates the measurements and likelihood for a given set of
         * parameters.  Stores them in output arguments.  GSL vector 
         * measurements gets newly allocated within the method.
         * 
         * Input: parameters
         * Outputs: measurements, likelihood
         * (abstract)
         */
        virtual void MeasurePoint(gsl_vector const* parameters,
                gsl_vector*& measurements,
                double& likelihood) = 0;


        std::vector<Mcmc::MarkovChain*> chains_;

        unsigned int const dimension_;
        unsigned int const num_chains_;
        unsigned int const max_steps_;
        double const burn_fraction_;

        unsigned int num_steps_;
        gsl_vector* last_points_mean_;
        gsl_matrix* last_points_covariance_;
        double last_points_covariance_det_;
        gsl_matrix* last_points_covariance_inv_;
    };

}

#endif	/* MCMC_MCMCSCAN_H */

