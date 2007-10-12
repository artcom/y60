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

#include "ColorDetection.h"

using namespace asl;
using namespace std;

namespace y60 {

    ColorDetection::ColorDetection(const std::string & theName) : 
        Algorithm(theName),
        _myResultNode("result"),
        _myThreshold(0), 
        _mySourceRaster(0)
    {}

    void 
    ColorDetection::configure(const dom::Node & theNode) {
		try {
			asl::fromString(theNode["threshold"].nodeValue(), _myThreshold);
		} catch(asl::Exception ex) {
            AC_ERROR << "ColorDetection::configure(): Could not parse configuration " << theNode;
		}
		AC_PRINT << "ColorDetection::configure threshold " << _myThreshold;
    }

	void 
	ColorDetection::onFrame(double t) {
    	const BGRRaster * myFrame = dom::dynamic_cast_Value<BGRRaster>(&*_mySourceRaster);
        std::vector<unsigned> myHistogram(360);
        Vector3f myHSV;
		for (BGRRaster::const_iterator it = myFrame->begin(); it != myFrame->end(); ++it) {
            rgb_to_hsl((*it)[2], (*it)[1], (*it)[0], myHSV);
            if (myHSV[1] > _myThreshold) {
                int myHue = int(floor(myHSV[0]));
                if (myHue >= 0 && myHue <= 360) {
                    myHistogram[myHue]++;
                }
            }
        }

        unsigned myMax = 0;
        unsigned myMin = UINT_MAX;
        for (unsigned i = 0; i < 360; ++i) {
            if (myHistogram[i] > myMax) {
                myMax = myHistogram[i];
            }
            if (myHistogram[i] < myMin) {
                myMin = myHistogram[i];
            }
        }
        
        _myResultNode("histogram")["maximum"] = as_string(myMax);
        _myResultNode("histogram")["minimum"] = as_string(myMin);
        _myResultNode("histogram")("#text").nodeValue(asl::as_string(myHistogram));
	}
}

