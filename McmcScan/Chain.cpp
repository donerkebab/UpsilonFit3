/* 
 * File:   Chain.cpp
 * Author: donerkebab
 * 
 * Created on March 12, 2014, 5:11 AM
 */

#include <memory>
#include <queue>
#include <string>

#include "Chain.h"

namespace McmcScan {

    Chain::Chain(std::shared_ptr<Point> const point,
            std::string const filename,
            unsigned int const buffer_size)
    : filename_(filename),
            buffer_size_(buffer_size),
            num_points_flushed_(0)
    {
        if ( point.get() == nullptr ) {
            throw std::invalid_argument("null point used to initialize chain");
        }
        if ( buffer_size == 0 ) {
            throw std::invalid_argument("cannot have zero buffer size");
        }
        
        buffer_.push(point);
    }

    std::string Chain::getFilename() const {
        return filename_;
    }
    
    unsigned int Chain::getBufferSize() const {
        return buffer_size_;
    }
    
    unsigned int Chain::getNumPointsBuffered() const {
        return buffer_.size();
    }
    
    unsigned int Chain::getNumPointsFlushed() const {
        return num_points_flushed_;
    }
    
    unsigned int Chain::getChainLength() const {
        return buffer_.size() + num_points_flushed_;
    }
    
    void Chain::Append(std::shared_ptr<Point> const point) {
        if ( point.get() == nullptr ) {
            throw std::invalid_argument("null point appended");
        }
        
        buffer_.push(point);
        
        if ( buffer_.size() >= buffer_size_ ) {
            Flush();
        }
    }
    
    void Chain::Flush() {
        //
    }
    
    
    
    Chain::~Chain() {
        // Need to flush all of the Point objects from the chain, including the
        // last one.  Save on coding complexity by adding a new dummy Point to 
        // the buffer, and then simply calling Flush().
        
        buffer_.push(std::shared_ptr<Point>(new Point(gsl_vector_alloc(1),
                gsl_vector_alloc(1), 0.) ) );
        Flush();
    }

}
