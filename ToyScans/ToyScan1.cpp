/* 
 * File:   ToyScan1.cpp
 * Author: donerkebab
 * 
 * Created on March 28, 2014, 8:45 AM
 */

#include "ToyScan1.h"

#include <cstdlib>
#include <cmath>

#include <stdexcept>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_sf_exp.h>
#include <gsl/gsl_vector.h>

#include "McmcScan.h"

namespace ToyScans {

    ToyScan1::ToyScan1(unsigned int num_chains,
            unsigned int max_steps,
            double burn_fraction,
            gsl_vector const* target_point,
            gsl_vector const* uncertainties)
    : Mcmc::McmcScan(3, num_chains, max_steps, burn_fraction) {
        if (target_point == nullptr || uncertainties == nullptr ||
                target_point->size != 3 || uncertainties->size != 3) {
            throw std::invalid_argument("need 3d vectors");
        }

        target_point_ = gsl_vector_alloc(3);
        gsl_vector_memcpy(target_point_, target_point);
        uncertainties_ = gsl_vector_alloc(3);
        gsl_vector_memcpy(uncertainties_, uncertainties);
    }

    ToyScan1::~ToyScan1() {
        gsl_vector_free(target_point_);
        gsl_vector_free(uncertainties_);
    }

    std::vector<std::pair<gsl_vector*, std::string> >
    ToyScan1::GenerateChainSeeds(unsigned int num_chains) {

        if (num_chains == 0) {
            throw std::invalid_argument("bad input to GenerateChainSeeds()");
        }

        std::vector<std::pair<gsl_vector*, std::string> > chains_info;

        for (int i_chain = 0; i_chain < num_chains; ++i_chain) {
            // Generate random parameters by expanding the random number in 
            // [0,1) to the full range [-10, 10)
            gsl_vector* seed_parameters = gsl_vector_alloc(3);
            for (int i_dimension = 0; i_dimension < 3; ++i_dimension) {
                gsl_vector_set(seed_parameters, i_dimension,
                        20.0 * gsl_rng_uniform(rng_) - 10.0);
            }

            std::stringstream filename_stream;
            filename_stream << "ToyScan1_chain" << i_chain + 1 << ".dat";

            chains_info.push_back(
                    std::pair<gsl_vector*, std::string>(seed_parameters,
                    filename_stream.str()));
        }

        return chains_info;
    }

    bool ToyScan1::IsValidParameters(gsl_vector const* parameters) {
        for (int i = 0; i < parameters->size; ++i) {
            if (gsl_vector_get(parameters, i) < -10.0 ||
                    gsl_vector_get(parameters, i) > 10.0) {
                return false;
            }
        }
        return true;
    }

    void ToyScan1::MeasurePoint(gsl_vector const* parameters,
            gsl_vector*& measurements,
            double& likelihood) {
        // Compute the displacement between the current point's parameters and
        // the target point's.
        gsl_vector* displacement = gsl_vector_alloc(3);
        gsl_vector_memcpy(displacement, parameters);
        gsl_vector_sub(displacement, target_point_);

        // Compute the measurements
        double distance = 0.0;
        for (int i = 0; i < 3; ++i) {
            distance += std::pow(gsl_vector_get(displacement, i), 2);
        }
        distance = std::sqrt(distance);
        double theta = std::acos(gsl_vector_get(displacement, 2) / distance);
        double phi = std::atan(gsl_vector_get(displacement, 1) /
                gsl_vector_get(displacement, 0));
        measurements = gsl_vector_alloc(3);
        gsl_vector_set(measurements, 0, distance);
        gsl_vector_set(measurements, 1, theta);
        gsl_vector_set(measurements, 2, phi);

        // Compute the likelihood
        likelihood = 1.0;
        for (int i = 0; i < 3; ++i) {
            likelihood *=
                    std::exp(-std::pow(gsl_vector_get(displacement, i) /
                    gsl_vector_get(uncertainties_, i), 2) / 2.0);
        }
    }


}

