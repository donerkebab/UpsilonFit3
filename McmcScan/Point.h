/* 
 * File:   Point.h
 * Author: donerkebab
 * 
 * Represents a point in the parameter space.  Stores the parameters and 
 * measurement values at that point, as well as the likelihood of the point.  
 * Parameters and measurements are stored as pointers to GSL vectors, which the
 * Point owns.  Parameters are meant to be stored here as their real-world 
 * values, and will have to be converted into the right form for the MCMC 
 * algorithm.
 * 
 * Immutable.  The constructor makes a defensive copy of the GSL vectors, and
 * accessor methods return pointers to const GSL vectors.  
 * 
 * Point objects are meant to be encapsulated by shared_ptr objects, so that we
 * will not be storing their data in multiple places.  As such, there is no copy
 * constructor or assignment operator.
 *  
 * When a Point object is deleted, it also deletes its GSL vectors.
 * 
 * Dev notes:
 * * Even though we do not accept NULL inputs, parameters and measurements are 
 *   still passed in as pointers, because they are always manipulated as 
 *   pointers in the GSL API.
 * * Considered keeping the data members const, but that conflicts with the 
 *   use of gsl_vector_memcpy() for defensive copies in the constructor. 
 *   Instead, const-ness is ensured by keeping all accessor methods const and
 *   having no mutator methods.
 *  
 * Created on March 9, 2014, 4:51 AM
 */

#ifndef MCMCSCAN_POINT_H
#define	MCMCSCAN_POINT_H

#include <gsl/gsl_vector.h>

namespace McmcScan {

    class Point {
    public:
        Point(gsl_vector* const parameters,
                gsl_vector* const measurements,
                double const likelihood);

        gsl_vector const* getParameters() const;
        gsl_vector const* getMeasurements() const;
        double getLikelihood() const;

        virtual ~Point();

    private:
        Point(Point const& orig);
        void operator=(Point const& orig);

        gsl_vector* parameters_;
        gsl_vector* measurements_;
        double likelihood_;
    };

}

#endif	/* MCMCSCAN_POINT_H */

