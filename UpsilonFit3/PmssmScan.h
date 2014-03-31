/* 
 * File:   PmssmScan.h
 * Author: donerkebab
 *
 * Created on March 31, 2014, 1:51 AM
 */

#ifndef UPSILONFIT3_PMSSMSCAN_H
#define	UPSILONFIT3_PMSSMSCAN_H

namespace UpsilonFit3 {

    class PmssmScan {
    public:
        PmssmScan();
        virtual ~PmssmScan();

    private:
        PmssmScan(PmssmScan const& orig);
        void operator=(PmssmScan const& orig);

    };

}

#endif	/* UPSILONFIT3_PMSSMSCAN_H */

