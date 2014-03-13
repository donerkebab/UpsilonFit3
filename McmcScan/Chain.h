/* 
 * File:   Chain.h
 * Author: donerkebab
 *
 * Represents a Markov chain of Point objects in the parameter space.  The chain
 * has a fixed buffer size and is tied to a text output file.  When the number
 * of buffered points reaches the buffer size, the chain flushes all except the
 * last point into the output file.  The user may also flush the chain manually.
 * 
 * If there are problems opening the output file, either when the Chain is
 * constructed or during flushing, it throws a McmcScan::ChainFlushError.  If
 * this happens during construction, the Chain will fail to initialize.  If this
 * happens during flushing, the user can ignore it, in which case the buffer
 * will simply go unflushed until the next attempt.
 * 
 * Chain doesn't actually store the Point objects directly, but rather 
 * shared_ptr pointers to Point objects.  This is because, in normal McmcScan 
 * use, the chain will contain many consecutive duplicate Point objects.
 * Storing them as pointers allows us to save considerable memory, and using
 * shared_ptr only frees the Point objects' memory when the last pointer is
 * flushed.
 * 
 * Terminology: "chain length" is considered to be the sum of the number of 
 * currently buffered points and the number of points already flushed.
 * 
 * Dev notes:
 * * I use a queue for the buffer, in order to internally mostly-enforce the
 *   rule that the system can only depend on the last point of the chain.  It
 *   also has the advantage of being based on deque, which can store elements in
 *   discontinuous memory blocks to avoid resizing/reallocation issues.
 * * Copy constructor is not supported because a copy of the chain would flush
 *   to the same output file.
 * 
 * Created on March 12, 2014, 5:11 AM
 */

#ifndef MCMCSCAN_CHAIN_H
#define	MCMCSCAN_CHAIN_H

#include <memory>
#include <queue>
#include <string>

#include "Point.h"
#include "ChainFlushError.h"

namespace McmcScan {
    
    class Chain {
    public:
        Chain(std::shared_ptr<McmcScan::Point> const point,
                std::string const filename,
                unsigned int const buffer_size);
        
        std::string getFilename() const;
        unsigned int getBufferSize() const;
        unsigned int getNumPointsBuffered() const;
        unsigned int getNumPointsFlushed() const;
        unsigned int getChainLength() const;
        
        std::shared_ptr<McmcScan::Point> getLastPoint() const;
        
        void Append(std::shared_ptr<McmcScan::Point> const point);
        void Flush();
        
        virtual ~Chain();

    private:
        Chain(Chain const& orig);

        std::queue<std::shared_ptr<McmcScan::Point> > buffer_;
        std::string const filename_;
        unsigned int const buffer_size_;
        unsigned int num_points_flushed_;
    };
    
}

#endif	/* MCMCSCAN_CHAIN_H */

