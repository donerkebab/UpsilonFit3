/*
 * File:   PointTest.h
 * Author: donerkebab
 *
 * Created on Mar 10, 2014, 10:21:34 AM
 */

#ifndef POINTTEST_H
#define	POINTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <gsl/gsl_vector.h>

class PointTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(PointTest);

    CPPUNIT_TEST(testPoint1);
    CPPUNIT_TEST(testPoint2);
    CPPUNIT_TEST(testPoint3);
    CPPUNIT_TEST(testPoint4);
    CPPUNIT_TEST(testPoint5);
    CPPUNIT_TEST(testPoint6);
    CPPUNIT_TEST(testGetLikelihood);
    CPPUNIT_TEST(testGetMeasurements);
    CPPUNIT_TEST(testGetParameters);

    CPPUNIT_TEST_SUITE_END();

public:
    PointTest();
    virtual ~PointTest();
    void setUp();
    void tearDown();

private:
    void testPoint1();
    void testPoint2();
    void testPoint3();
    void testPoint4();
    void testPoint5();
    void testPoint6();
    void testGetLikelihood();
    void testGetMeasurements();
    void testGetParameters();
    
    gsl_vector* parameters_;
    gsl_vector* measurements_;
    double likelihood_;
};

#endif	/* POINTTEST_H */

