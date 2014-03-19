/*
 * File:   MarkovChainTestClass.cpp
 * Author: donerkebab
 *
 * Created on Mar 12, 2014, 3:49:33 PM
 */

#include "MarkovChainTestClass.h"

#include <cstdio>

#include <memory>
#include <stdexcept>
#include <string>

#include "../Point.h"
#include "../MarkovChain.h"


CPPUNIT_TEST_SUITE_REGISTRATION(MarkovChainTestClass);

MarkovChainTestClass::MarkovChainTestClass()
: dummy_output_filename_("dummy_output_file.dat"),
        d_(1E-5)
{
}

MarkovChainTestClass::~MarkovChainTestClass() {
}

void MarkovChainTestClass::setUp() {
}

void MarkovChainTestClass::tearDown() {
    std::remove(dummy_output_filename_.c_str());
}

void MarkovChainTestClass::testChainInitFails() {
    gsl_vector* params = gsl_vector_calloc(2);
    gsl_vector* meas = gsl_vector_calloc(3);
    double like = 0.2;
    std::shared_ptr<McmcScan::Point> point(
            new McmcScan::Point(params, meas, like));
    unsigned int buffer_size = 3;
    
    CPPUNIT_ASSERT_THROW(McmcScan::MarkovChain chain(nullptr, 
            dummy_output_filename_, buffer_size), std::invalid_argument);
    CPPUNIT_ASSERT_THROW(McmcScan::MarkovChain chain(point, "", buffer_size),
            std::invalid_argument);
    CPPUNIT_ASSERT_THROW(McmcScan::MarkovChain chain(point, 
            dummy_output_filename_, 0), std::invalid_argument);
}

void MarkovChainTestClass::testChainFill() {
    // Initialize the chain with the first point
    gsl_vector* params1 = gsl_vector_calloc(2);
    gsl_vector* meas1 = gsl_vector_calloc(3);
    double like1 = 0.2;
    std::shared_ptr<McmcScan::Point> point1(
            new McmcScan::Point(params1, meas1, like1));
    unsigned int buffer_size = 3;

    McmcScan::MarkovChain chain(point1, dummy_output_filename_, buffer_size);

    CPPUNIT_ASSERT(chain.filename() == dummy_output_filename_);
    CPPUNIT_ASSERT(chain.buffer_size() == buffer_size);
    CPPUNIT_ASSERT(chain.num_points_buffered() == 1);
    CPPUNIT_ASSERT(chain.num_points_flushed() == 0);
    CPPUNIT_ASSERT(chain.length() == 1);
    CPPUNIT_ASSERT(chain.last_point() == point1);
    
    // Add the second point
    gsl_vector* params2 = gsl_vector_alloc(2);
    gsl_vector_set(params2, 0, 0.0);
    gsl_vector_set(params2, 1, 1.1);
    gsl_vector* meas2 = gsl_vector_alloc(3);
    gsl_vector_set(meas2, 0, 0.1);
    gsl_vector_set(meas2, 1, 1.2);
    gsl_vector_set(meas2, 2, 2.3);
    double like2 = 0.56;
    std::shared_ptr<McmcScan::Point> point2(
            new McmcScan::Point(params2, meas2, like2));
    chain.Append(point2);
    
    CPPUNIT_ASSERT(chain.num_points_buffered() == 2);
    CPPUNIT_ASSERT(chain.num_points_flushed() == 0);
    CPPUNIT_ASSERT(chain.length() == 2);
    CPPUNIT_ASSERT(chain.last_point() == point2);
    
    // Add the third point as a duplicate of the second point
    // Flush() should have been called at the end of Append()
    // But point2's Point object should not have been deleted yet
    chain.Append(point2);
    
    CPPUNIT_ASSERT(chain.num_points_buffered() == 1);
    CPPUNIT_ASSERT(chain.num_points_flushed() == 2);
    CPPUNIT_ASSERT(chain.length() == 3);
    CPPUNIT_ASSERT(chain.last_point() == point2);
    
    CPPUNIT_ASSERT(gsl_vector_get(chain.last_point()->parameters(), 1) 
            == gsl_vector_get(params2, 1));
    
    // Check what has been output
    std::FILE* read_output = std::fopen(dummy_output_filename_.c_str(), "r");
    float read_buffer;
    std::fscanf(read_output, "%f", &read_buffer);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(read_buffer, gsl_vector_get(params1, 0), d_);
    std::fscanf(read_output, "%f", &read_buffer);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(read_buffer, gsl_vector_get(params1, 1), d_);
    std::fscanf(read_output, "%f", &read_buffer);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(read_buffer, gsl_vector_get(meas1, 0), d_);
    std::fscanf(read_output, "%f", &read_buffer);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(read_buffer, gsl_vector_get(meas1, 1), d_);
    std::fscanf(read_output, "%f", &read_buffer);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(read_buffer, gsl_vector_get(meas1, 2), d_);
    std::fscanf(read_output, "%f", &read_buffer);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(read_buffer, like1, d_);
    std::fscanf(read_output, "%f", &read_buffer);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(read_buffer, gsl_vector_get(params2, 0), d_);
    std::fscanf(read_output, "%f", &read_buffer);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(read_buffer, gsl_vector_get(params2, 1), d_);
    std::fscanf(read_output, "%f", &read_buffer);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(read_buffer, gsl_vector_get(meas2, 0), d_);
    std::fscanf(read_output, "%f", &read_buffer);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(read_buffer, gsl_vector_get(meas2, 1), d_);
    std::fscanf(read_output, "%f", &read_buffer);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(read_buffer, gsl_vector_get(meas2, 2), d_);
    std::fscanf(read_output, "%f", &read_buffer);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(read_buffer, like2, d_);
    std::fscanf(read_output, "%f", &read_buffer);
    CPPUNIT_ASSERT(std::feof(read_output) != 0);
    std::fclose(read_output);
    
    // Try flushing at this point: nothing should happen
    chain.Flush();
    
    CPPUNIT_ASSERT(chain.num_points_buffered() == 1);
    CPPUNIT_ASSERT(chain.num_points_flushed() == 2);
    CPPUNIT_ASSERT(chain.length() == 3);
    CPPUNIT_ASSERT(chain.last_point() == point2);
    
    // Add a fourth point
    gsl_vector* params4 = gsl_vector_alloc(2);
    gsl_vector_set(params4, 0, 2.2);
    gsl_vector_set(params4, 1, 3.3);
    gsl_vector* meas4 = gsl_vector_alloc(3);
    gsl_vector_set(meas4, 0, 1.0);
    gsl_vector_set(meas4, 1, 2.1);
    gsl_vector_set(meas4, 2, 3.2);
    double like4 = 10.3;
    std::shared_ptr<McmcScan::Point> point4(
            new McmcScan::Point(params4, meas4, like4));
    chain.Append(point4);
    
    CPPUNIT_ASSERT(chain.num_points_buffered() == 2);
    CPPUNIT_ASSERT(chain.num_points_flushed() == 2);
    CPPUNIT_ASSERT(chain.length() == 4);
    CPPUNIT_ASSERT(chain.last_point() == point4);
}




