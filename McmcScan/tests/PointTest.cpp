/*
 * File:   PointTest.cpp
 * Author: donerkebab
 *
 * Created on Mar 10, 2014, 10:21:36 AM
 */

#include "PointTest.h"
#include "../Point.h"

CPPUNIT_TEST_SUITE_REGISTRATION(PointTest);

PointTest::PointTest() {
}

PointTest::~PointTest() {
}

void PointTest::setUp() {
    parameters_ = gsl_vector_alloc(3);
    gsl_vector_set(parameters_, 0, 0.);
    gsl_vector_set(parameters_, 1, -1.);
    gsl_vector_set(parameters_, 2, 2.);
    measurements_ = gsl_vector_alloc(4);
    gsl_vector_set(measurements_, 0, -10.4);
    gsl_vector_set(measurements_, 1, 56.1);
    gsl_vector_set(measurements_, 2, -22.35);
    gsl_vector_set(measurements_, 3, 0.0003);
    likelihood_ = 0.6;
}

void PointTest::tearDown() {
    // Nothing I can do.  The memory may or may not already be freed, but
    // because of how GSL vectors are handled, there is no way for me to know
    // and no error-safe way for me to try.
}

void PointTest::testPoint1() {
    CPPUNIT_ASSERT_NO_THROW(McmcScan::Point point(parameters_, measurements_, 
            likelihood_));
}

void PointTest::testPoint2() {
    CPPUNIT_ASSERT_NO_THROW(McmcScan::Point point(parameters_, measurements_, 
            0.));
}

void PointTest::testPoint3() {
    CPPUNIT_ASSERT_THROW(McmcScan::Point point(NULL, measurements_, 
            likelihood_), std::invalid_argument);
}

void PointTest::testPoint4() {
    CPPUNIT_ASSERT_THROW(McmcScan::Point point(parameters_, NULL, likelihood_),
            std::invalid_argument);
}

void PointTest::testPoint5() {
    CPPUNIT_ASSERT_THROW(McmcScan::Point point(parameters_, measurements_, 
            -0.6), std::invalid_argument);
}

void PointTest::testPoint6() {
    McmcScan::Point point1(parameters_, measurements_, likelihood_);
    McmcScan::Point point2(point1);

    CPPUNIT_ASSERT(gsl_vector_equal(point1.getParameters(), 
            point2.getParameters()) == 1);
    CPPUNIT_ASSERT(gsl_vector_equal(point1.getMeasurements(),
            point2.getMeasurements()) == 1);
    CPPUNIT_ASSERT(point1.getLikelihood() == point2.getLikelihood());

    // Test for deep copy
    gsl_vector_set(parameters_, 0, 55.);
    CPPUNIT_ASSERT(gsl_vector_get(point1.getParameters(), 0) == 55.);
    CPPUNIT_ASSERT(gsl_vector_get(point2.getParameters(), 0) == 0.);
}

void PointTest::testGetLikelihood() {
    McmcScan::Point point(parameters_, measurements_, likelihood_);
    CPPUNIT_ASSERT(point.getLikelihood() == likelihood_);
}

void PointTest::testGetMeasurements() {
    McmcScan::Point point(parameters_, measurements_, likelihood_);
    CPPUNIT_ASSERT(gsl_vector_equal(point.getMeasurements(), measurements_)
            == 1);
}

void PointTest::testGetParameters() {
    McmcScan::Point point(parameters_, measurements_, likelihood_);
    CPPUNIT_ASSERT(gsl_vector_equal(point.getParameters(), parameters_)
            == 1);
}

