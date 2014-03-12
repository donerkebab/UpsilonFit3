/* 
 * File:   Point.cpp
 * Author: donerkebab
 * 
 * Created on March 9, 2014, 4:51 AM
 */

#include <stdexcept>

#include "Point.h"

namespace McmcScan {

    Point::Point(gsl_vector* const parameters, 
            gsl_vector* const measurements,
            double const likelihood) {
        if (parameters == nullptr) {
            throw std::invalid_argument("input parameters is null");
        }
        if (measurements == nullptr) {
            throw std::invalid_argument("input measurements is null");
        }
        if (likelihood < 0) {
            throw std::invalid_argument("input likelihood is negative");
        }
        
        parameters_ = gsl_vector_alloc(parameters->size);
        gsl_vector_memcpy(parameters_, parameters);
        measurements_ = gsl_vector_alloc(measurements->size);
        gsl_vector_memcpy(measurements_, measurements);
        likelihood_ = likelihood;
    }

    Point::Point(Point const& orig) {
        parameters_ = gsl_vector_alloc(orig.parameters_->size);
        gsl_vector_memcpy(parameters_, orig.parameters_);
        measurements_ = gsl_vector_alloc(orig.measurements_->size);
        gsl_vector_memcpy(measurements_, orig.measurements_);
        likelihood_ = orig.likelihood_;
    }

    gsl_vector const* Point::getParameters() const {
        return parameters_;
    }

    gsl_vector const* Point::getMeasurements() const {
        return measurements_;
    }

    double Point::getLikelihood() const {
        return likelihood_;
    }

    Point::~Point() {
        gsl_vector_free(parameters_);
        gsl_vector_free(measurements_);
    }

}