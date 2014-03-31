/* 
 * File:   ToyScan2.cpp
 * Author: donerkebab
 * 
 * Created on March 30, 2014, 12:43 PM
 */

#include "ToyScan2.h"

#include <cstdlib>
#include <cmath>

#include <stdexcept>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_vector.h>

#include "McmcScan.h"

namespace ToyScans {

    ToyScan2::ToyScan2(unsigned int num_chains,
            unsigned int max_steps,
            double burn_fraction,
            gsl_vector const* center_point,
            double radius,
            double uncertainty)
    : Mcmc::McmcScan(2, num_chains, max_steps, burn_fraction) {
        if (center_point == nullptr || center_point->size != 2 ||
                radius < 0.0 || uncertainty <= 0.0) {
            throw std::invalid_argument("invalid input to ToyScan2");
        }

        center_point_ = gsl_vector_alloc(2);
        gsl_vector_memcpy(center_point_, center_point);
        radius_ = radius;
        uncertainty_ = uncertainty;
    }

    ToyScan2::~ToyScan2() {
        gsl_vector_free(center_point_);
    }

    std::vector<std::pair<gsl_vector*, std::string> >
    ToyScan2::GenerateChainSeeds(unsigned int num_chains) {

        if (num_chains == 0) {
            throw std::invalid_argument("bad input to GenerateChainSeeds()");
        }

        std::vector<std::pair<gsl_vector*, std::string> > chains_info;

        for (int i_chain = 0; i_chain < num_chains; ++i_chain) {
            // Generate random parameters by expanding the random number in 
            // [0,1) to the full range [-10, 10)
            gsl_vector* seed_parameters = gsl_vector_alloc(2);
            for (int i_dimension = 0; i_dimension < 2; ++i_dimension) {
                gsl_vector_set(seed_parameters, i_dimension,
                        20.0 * gsl_rng_uniform(rng_) - 10.0);
            }

            std::stringstream filename_stream;
            filename_stream << "ToyScan2_chain" << i_chain + 1 << ".dat";

            chains_info.push_back(
                    std::pair<gsl_vector*, std::string>(seed_parameters,
                    filename_stream.str()));
        }

        return chains_info;
    }

    bool ToyScan2::IsValidParameters(gsl_vector const* parameters) {
        for (int i = 0; i < parameters->size; ++i) {
            if (gsl_vector_get(parameters, i) < -10.0 ||
                    gsl_vector_get(parameters, i) > 10.0) {
                return false;
            }
        }
        return true;
    }

    void ToyScan2::MeasurePoint(gsl_vector const* parameters,
            gsl_vector*& measurements,
            double& likelihood) {
        // Compute the displacement between the current point's parameters and
        // the center point's.
        gsl_vector* displacement = gsl_vector_alloc(2);
        gsl_vector_memcpy(displacement, parameters);
        gsl_vector_sub(displacement, center_point_);

        // Compute the measurements
        double distance = 0.0;
        for (int i = 0; i < 2; ++i) {
            distance += std::pow(gsl_vector_get(displacement, i), 2);
        }
        distance = std::sqrt(distance);
        double theta = std::acos(gsl_vector_get(displacement, 0) / distance);
        measurements = gsl_vector_alloc(2);
        gsl_vector_set(measurements, 0, distance);
        gsl_vector_set(measurements, 1, theta);

        // Compute the likelihood
        likelihood = std::exp(-std::pow((distance - radius_) / uncertainty_, 2)
                / 2.0);
    }


}


