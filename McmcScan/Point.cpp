/* 
 * File:   Point.cpp
 * Author: donerkebab
 * 
 * Created on March 9, 2014, 4:51 AM
 */

#include <stdexcept>

#include "Point.h"

namespace McmcScan {

    Point::Point(gsl_vector* parameters, 
            gsl_vector* measurements,
            double likelihood) 
    : parameters_(parameters), 
            measurements_(measurements), 
            likelihood_(likelihood)
    {
        if (parameters_ == NULL) {
            throw std::invalid_argument("input parameters is null");
        }
        if (measurements_ == NULL) {
            throw std::invalid_argument("input measurements is null");
        }
        if (likelihood_ < 0) {
            throw std::invalid_argument("input likelihood is negative");
        }
    }

    Point::Point(const Point& orig) {
        parameters_ = gsl_vector_alloc(orig.parameters_->size);
        gsl_vector_memcpy(parameters_, orig.parameters_);
        measurements_ = gsl_vector_alloc(orig.measurements_->size);
        gsl_vector_memcpy(measurements_, orig.measurements_);
        likelihood_ = orig.likelihood_;

        // Must do the copy with a temporary variable, because the data member
        // GSL vectors are const objects.
/*        gsl_vector* params_temp = gsl_vector_alloc(orig.parameters_->size);
        gsl_vector_memcpy(params_temp, orig.parameters_);
        parameters_ = params_temp;
        
        gsl_vector* meas_temp = gsl_vector_alloc(orig.measurements_->size);
        gsl_vector_memcpy(meas_temp, orig.measurements_);
        measurements_ = meas_temp;

        likelihood_ = orig.likelihood_;*/
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