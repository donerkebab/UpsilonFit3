/*
 * File:   PointTest.cpp
 * Author: donerkebab
 *
 * Created on Mar 10, 2014, 10:21:36 AM
 */

#include "PointTest.h"

#include <stdexcept>

#include <gsl/gsl_vector.h>

#include "../Point.h"

CPPUNIT_TEST_SUITE_REGISTRATION(PointTest);

PointTest::PointTest()
: d_(1E-5)
{
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
    gsl_vector_free(parameters_);
    gsl_vector_free(measurements_);
}

void PointTest::testInitialization() {
    CPPUNIT_ASSERT_NO_THROW(McmcScan::Point(parameters_, measurements_, 
            likelihood_));
    CPPUNIT_ASSERT_NO_THROW(McmcScan::Point(parameters_, measurements_, 0.));
    
    CPPUNIT_ASSERT_THROW(McmcScan::Point(nullptr, measurements_, likelihood_),
            std::invalid_argument);
    CPPUNIT_ASSERT_THROW(McmcScan::Point(parameters_, nullptr, likelihood_),
            std::invalid_argument);
    CPPUNIT_ASSERT_THROW(McmcScan::Point(parameters_, measurements_, -0.6), 
            std::invalid_argument); 
}

void PointTest::testDefensiveCopy() {
    // Test for defensive copy on construction
    McmcScan::Point point(parameters_, measurements_, likelihood_);
    
    double old_value = gsl_vector_get(parameters_, 0);
    gsl_vector_set(parameters_, 0, 55.);
    
    CPPUNIT_ASSERT_DOUBLES_EQUAL(gsl_vector_get(point.getParameters(), 0), 
            old_value, d_);
}

void PointTest::testAccessors() {
    McmcScan::Point point(parameters_, measurements_, likelihood_);
    
    CPPUNIT_ASSERT(gsl_vector_equal(point.getParameters(), parameters_) == 1);
    CPPUNIT_ASSERT(gsl_vector_equal(point.getMeasurements(), measurements_) 
            == 1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point.getLikelihood(), likelihood_, d_);
}
