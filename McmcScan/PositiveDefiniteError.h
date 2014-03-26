/* 
 * File:   PositiveDefiniteError.h
 * Author: donerkebab
 * 
 * Exception for errors when a matrix becomes no longer positive definite.
 * Extends std::runtime_error.
 *
 * Created on March 25, 2014, 2:58 PM
 */

#ifndef MCMC_POSITIVEDEFINITEERROR_H
#define	MCMC_POSITIVEDEFINITEERROR_H

#include <stdexcept>

namespace Mcmc {
    
    class PositiveDefiniteError : public std::runtime_error {
    public:
        PositiveDefiniteError() 
        : std::runtime_error("covariance matrix is no longer positive definite")
        {}       
    };
    
}

#endif	/* MCMC_POSITIVEDEFINITEERROR_H */

