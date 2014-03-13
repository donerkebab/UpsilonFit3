/*
 * File:   PointTest.h
 * Author: donerkebab
 *
 * Created on Mar 10, 2014, 10:21:34 AM
 */

#ifndef MCMCSCAN_POINTTEST_H
#define	MCMCSCAN_POINTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <gsl/gsl_vector.h>

class PointTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(PointTest);

    CPPUNIT_TEST(testInitialization);
    CPPUNIT_TEST(testDefensiveCopy);
    CPPUNIT_TEST(testAccessors);
    
    CPPUNIT_TEST_SUITE_END();

public:
    PointTest();
    virtual ~PointTest();
    void setUp();
    void tearDown();

private:
    void testInitialization();
    void testDefensiveCopy();
    void testAccessors();
    
    gsl_vector* parameters_;
    gsl_vector* measurements_;
    double likelihood_;
    
    double const d_;  // delta for CPPUNIT_ASSERT_DOUBLES_EQUAL
};

#endif	/* MCMCSCAN_POINTTEST_H */

