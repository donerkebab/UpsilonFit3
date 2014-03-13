/*
 * File:   ChainTestClass.h
 * Author: donerkebab
 *
 * Created on Mar 12, 2014, 3:49:32 PM
 */

#ifndef CHAINTESTCLASS_H
#define	CHAINTESTCLASS_H

#include <string>
#include <cppunit/extensions/HelperMacros.h>

class ChainTestClass : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(ChainTestClass);

    CPPUNIT_TEST(testChainInitFails);
    CPPUNIT_TEST(testChainFill);
    
    CPPUNIT_TEST_SUITE_END();

public:
    ChainTestClass();
    virtual ~ChainTestClass();
    void setUp();
    void tearDown();

private:
    void testChainInitFails();
    void testChainFill();

    
    
    std::string dummy_output_filename_;
};

#endif	/* CHAINTESTCLASS_H */

