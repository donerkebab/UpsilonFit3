/* 
 * File:   Point.h
 * Author: donerkebab
 * 
 * Represents a point in the parameter space.  Also stores the values of 
 * measurements at the point, as well as the likelihood of the point. Parameters
 * and measurements are stored as pointers to GSL vectors.  
 * 
 * Immutable.  The copy constructor does a deep copy of the pointers, although
 * since Point objects are immutable, it is suggested that one simply create a
 * shared_ptr to the Point object if another copy is needed.
 * 
 * The GSL vectors are owned by the Point object storing them.  They can only be
 * accessed as const objects.  When a Point object is deleted, it also deletes 
 * its GSL vectors.
 * 
 * Dev notes:
 * * Even though we do not accept NULL inputs, parameters and measurements are 
 *   still passed in as pointers, because they are always manipulated as 
 *   pointers in the GSL API.
 * * Considered keeping the data members const, but that conflicts with the 
 *   ability to do a deep copy of the GSL vectors for the copy constructor.  So
 *   instead, const-ness is ensured by keeping all accessing methods const.
 * * Considered using shared_ptr for the GSL vectors, but for now it sounds like
 *   overkill, especially if this class will mostly just be used within the
 *   McmcScan package.
 * 
 * Created on March 9, 2014, 4:51 AM
 */

#ifndef POINT_H
#define	POINT_H

#include <gsl/gsl_vector.h>

namespace McmcScan {

    class Point {
    public:
        Point(gsl_vector* parameters,
                gsl_vector* measurements,
                double likelihood);
        Point(const Point& orig);

        gsl_vector const* getParameters() const;
        gsl_vector const* getMeasurements() const;
        double getLikelihood() const;

        virtual ~Point();

    private:
        gsl_vector* parameters_;
        gsl_vector* measurements_;
        double likelihood_;
    };

}

#endif	/* POINT_H */

