/* 
 * File:   MarkovChain.cpp
 * Author: donerkebab
 * 
 * Created on March 12, 2014, 5:11 AM
 */

#include "MarkovChain.h"

#include <cstdio>

#include <memory>
#include <queue>
#include <string>

#include "Point.h"
#include "ChainFlushError.h"

namespace McmcScan {
    
    MarkovChain::MarkovChain(std::shared_ptr<McmcScan::Point> const point,
            std::string const filename,
            unsigned int const buffer_size)
    : filename_(filename),
            buffer_size_(buffer_size),
            num_points_flushed_(0)
    {
        if ( point.get() == nullptr ) {
            throw std::invalid_argument("null point used to initialize chain");
        }
        if ( filename == "" ) {
            throw std::invalid_argument("invalid filename");
        }
        if ( buffer_size == 0 ) {
            throw std::invalid_argument("cannot have zero buffer size");
        }
        
        // Also check to see if there are any issues opening the output file
        std::FILE* output_file = std::fopen(filename_.c_str(), "a");
        if ( output_file == nullptr ) {
            throw McmcScan::ChainFlushError();
        } else {
            std::fclose(output_file);
        }

        buffer_.push(point);        

    }

    MarkovChain::~MarkovChain() {
        // Need to flush all of the Point objects from the chain, including the
        // last one.  Save on coding complexity by adding a new dummy Point to 
        // the buffer, and then simply calling Flush().
        
        buffer_.push(std::shared_ptr<McmcScan::Point>(
                new Point(gsl_vector_alloc(1), gsl_vector_alloc(1), 0.) ) );
        Flush();
    }

    std::string MarkovChain::filename() const {
        return filename_;
    }
    
    unsigned int MarkovChain::buffer_size() const {
        return buffer_size_;
    }
    
    unsigned int MarkovChain::num_points_buffered() const {
        return buffer_.size();
    }
    
    unsigned int MarkovChain::num_points_flushed() const {
        return num_points_flushed_;
    }
    
    unsigned int MarkovChain::length() const {
        return buffer_.size() + num_points_flushed_;
    }
    
    std::shared_ptr<McmcScan::Point> MarkovChain::last_point() const {
        return buffer_.back();
    }
    
    void MarkovChain::Append(std::shared_ptr<McmcScan::Point> const point) {
        if ( point.get() == nullptr ) {
            throw std::invalid_argument("null point appended");
        }
        
        buffer_.push(point);
        
        if ( buffer_.size() >= buffer_size_ ) {
            Flush();
        }
    }
    
    void MarkovChain::Flush() {
        if ( buffer_.size() == 1 ) {
            return;
        }
        
        std::FILE* output_file = std::fopen(filename_.c_str(), "a");
        if ( output_file == nullptr ) {
            throw McmcScan::ChainFlushError();
        } else {
            while ( buffer_.size() > 1 ) {
                std::shared_ptr<McmcScan::Point> point = buffer_.front();
                
                gsl_vector const* parameters = point->parameters();
                for ( int i = 0; i < parameters->size; ++i ) {
                    std::fprintf(output_file, "%- 9.8E  ", 
                            gsl_vector_get(parameters, i));
                }
                std::fprintf(output_file, "\n");
                
                gsl_vector const* measurements = point->measurements();
                for ( int i = 0; i < measurements->size; ++i) {
                    std::fprintf(output_file, "%- 9.8E  ", 
                            gsl_vector_get(measurements, i));
                }
                std::fprintf(output_file, "\n");
                
                std::fprintf(output_file, "%- 9.8E\n", point->likelihood());
                std::fprintf(output_file, "\n");
                
                buffer_.pop();
                ++num_points_flushed_;
            }
            
            std::fclose(output_file);
        }
    }   
        
}
