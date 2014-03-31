/* 
 * File:   PmssmScan.h
 * Author: donerkebab
 *
 * Created on March 31, 2014, 1:51 AM
 */

#ifndef UPSILONFIT3_PMSSMSCAN_H
#define	UPSILONFIT3_PMSSMSCAN_H

#include <vector>

#include <gsl/gsl_vector.h>

#include "McmcScan.h"

namespace UpsilonFit3 {

    class PmssmScan : public Mcmc::McmcScan {
    public:
        PmssmScan(unsigned int num_chains,
                unsigned int max_steps,
                double burn_fraction,
                gsl_vector const* benchmark_sm,
                gsl_vector const* benchmark_msugra,
                std::vector<unsigned int> parameter_key);
        virtual ~PmssmScan();

/*        std::vector<std::pair<gsl_vector*, std::string> >
        GenerateChainSeeds(unsigned int num_chains,
                unsigned int max_tries_per_direction,
                double initial_step_guess,
                std::pair<double, double> likelihood_bounds);
*/
    private:
        PmssmScan(PmssmScan const& orig);
        void operator=(PmssmScan const& orig);

//        gsl_vector* ConvertMsugraToPmssm(gsl_vector const*);
        
//        bool IsValidParameters(gsl_vector const* parameters);
        
//        bool IsValidSpectrum();

/*        void MeasurePoint(gsl_vector const* parameters,
                gsl_vector*& measurements,
                double& likelihood);
*/
        
        gsl_vector* benchmark_sm_;
        gsl_vector* benchmark_msugra_;
        gsl_vector* benchmark_pmssm_;
        
        std::vector<unsigned int> parameter_key_;
    };

}

#endif	/* UPSILONFIT3_PMSSMSCAN_H */

