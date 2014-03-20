/*
 * File:   MarkovChainTestClass.h
 * Author: donerkebab
 *
 * Created on Mar 12, 2014, 3:49:32 PM
 */

#ifndef MCMC_MARKOVCHAINTESTCLASS_H
#define	MCMC_MARKOVCHAINTESTCLASS_H

#include <string>

#include <cppunit/extensions/HelperMacros.h>

class MarkovChainTestClass : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(MarkovChainTestClass);

    CPPUNIT_TEST(testChainInitFails);
    CPPUNIT_TEST(testChainFill);
    
    CPPUNIT_TEST_SUITE_END();

public:
    MarkovChainTestClass();
    virtual ~MarkovChainTestClass();
    void setUp();
    void tearDown();

private:
    void testChainInitFails();
    void testChainFill();

    std::string const dummy_output_filename_;
    double const d_;  // delta for CPPUNIT_ASSERT_DOUBLES_EQUAL
};


#endif	/* MCMC_MARKOVCHAINTESTCLASS_H */

