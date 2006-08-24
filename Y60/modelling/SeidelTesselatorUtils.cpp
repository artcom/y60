//============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
#include "SeidelTesselatorUtils.h"

#ifdef WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN 1
    #endif
    #include <windows.h>
    #undef max
    #include <math.h>
#endif

#define DB(x) // x
#define DB2(x) // x


namespace y60 {
    /* Get log*n for given n */
    int math_logstar_n(int n) {
        int i;
        double v;
        
        for (i = 0, v = (double) n; v >= 1; i++) {
            v = log(v)/log(2.0f);
        }      
        return (i - 1);
    }
      
    
    int math_N(int n, int h) {
        int i;
        double v;
    
        for (i = 0, v = (int) n; i < h; i++) {
            v = log(v)/log(2.0f);
        }      
        return (int) ceil((double) 1.0*n/v);
    }

    /* Return the next segment in the generated random ordering of all the */
    /* segments in S */
    int choose_segment()
    {
        static int choose_idx = 1;
        return choose_idx++;//permute[choose_idx++];
    }

}
