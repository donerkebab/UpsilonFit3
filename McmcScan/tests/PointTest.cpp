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
    gsl_vector_free(parameters_);
    gsl_vector_free(measurements_);
}

void PointTest::testPoint1() {
    CPPUNIT_ASSERT_NO_THROW(McmcScan::Point point1(parameters_, measurements_, 
            likelihood_));
    
    // Test for defensive copy
    McmcScan::Point point2(parameters_, measurements_, likelihood_);
    gsl_vector_set(parameters_, 0, 55.);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(gsl_vector_get(point2.getParameters(), 0), 0.,
            d_);
}

void PointTest::testPoint2() {
    CPPUNIT_ASSERT_NO_THROW(McmcScan::Point point(parameters_, measurements_, 
            0.));
}

void PointTest::testPoint3() {
    CPPUNIT_ASSERT_THROW(McmcScan::Point point(nullptr, measurements_, 
            likelihood_), std::invalid_argument);
}

void PointTest::testPoint4() {
    CPPUNIT_ASSERT_THROW(McmcScan::Point point(parameters_, nullptr, 
            likelihood_), std::invalid_argument);
}

void PointTest::testPoint5() {
    CPPUNIT_ASSERT_THROW(McmcScan::Point point(parameters_, measurements_, 
            -0.6), std::invalid_argument);
}

void PointTest::testPoint6() {
    McmcScan::Point* point1 = new McmcScan::Point(parameters_, measurements_, 
            likelihood_);
    McmcScan::Point point2(*point1);

    CPPUNIT_ASSERT(gsl_vector_equal(point1->getParameters(), 
            point2.getParameters()) == 1);
    CPPUNIT_ASSERT(gsl_vector_equal(point1->getMeasurements(),
            point2.getMeasurements()) == 1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point1->getLikelihood(), 
            point2.getLikelihood(), d_);
    
    // Test for deep copy
    delete point1;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(gsl_vector_get(point2.getParameters(), 0),
            0., d_);
}

void PointTest::testGetLikelihood() {
    McmcScan::Point point(parameters_, measurements_, likelihood_);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(point.getLikelihood(), likelihood_, d_);
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

