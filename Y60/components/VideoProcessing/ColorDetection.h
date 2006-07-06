//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_ColorDetection_h_
#define _ac_ColorDetection_h_

#include "Algorithm.h"

namespace y60 {

	class ColorDetection : public Algorithm {
		public:
            ColorDetection();

			static std::string getName() { return "colordetection"; }
		    void onFrame(dom::ValuePtr theRaster, double t);

            void configure(const dom::Node & theNode);
	        const dom::Node & result() const { 
		        return _myResultNode;
	        }
		private:
			void rgb_to_hsl(unsigned char theR, unsigned char theG, unsigned char theB, asl::Vector3f & theResult);
            dom::Element _myResultNode;
            float        _myThreshold;
	};

    inline
    void
    ColorDetection::rgb_to_hsl(unsigned char theR, unsigned char theG, unsigned char theB, asl::Vector3f & theResult) {       
	    unsigned char myMax = asl::maximum(theR, asl::maximum(theG, theB));
        unsigned char myMin = asl::minimum(theR, asl::minimum(theG, theB));
        float myDelta = float(myMax - myMin);
        
        theResult[0] = 0;    // should be undefined (for grey colors)    
	    theResult[1] = myMax ? (255 * myDelta / myMax) : 0;
        theResult[2] = (myMin + myMax) / 2.0f;
        
        if (theResult[1]) {            
            if (theR == myMax) {
                theResult[0] = (theG - theB) / myDelta;
            } else if (theG == myMax) {
                theResult[0] = 2 + (theB - theR) / myDelta;
            } else if (theB == myMax) {
                theResult[0] = 4 + (theR - theG) / myDelta;
            }
            theResult[0] *= 60;
            if (theResult[0] < 0){
                theResult[0] += 360;    
            }
        }   
    }

}

#endif

