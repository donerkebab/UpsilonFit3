/* 
 * File:   ChainFlushError.h
 * Author: donerkebab
 *
 * Exception for errors when Chain tries to flush its buffer to a data file.
 * Extends std::runtime_error.
 * 
 *
 * Created on March 13, 2014, 4:19 AM
 */

#ifndef MCMCSCAN_CHAINFLUSHERROR_H
#define	MCMCSCAN_CHAINFLUSHERROR_H

#include <stdexcept>

namespace McmcScan {
    
    class ChainFlushError : public std::runtime_error {
    public:
        ChainFlushError() 
        : std::runtime_error("could not open file for chain buffer flushing")
        {}
    };
    
}


#endif	/* MCMCSCAN_CHAINFLUSHERROR_H */

