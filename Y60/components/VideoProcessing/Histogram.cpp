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

#include "Histogram.h"

using namespace asl;
using namespace std;
using namespace dom;

namespace y60 {

    Histogram::Histogram() : 
        _myResultNode("result")
    {
        _myResultNode.appendChild(Element("red"));
        _myResultNode.childNode("red")->appendChild(Text(""));
        _myResultNode.appendChild(Element("green"));
        _myResultNode.childNode("green")->appendChild(Text(""));
        _myResultNode.appendChild(Element("blue"));
        _myResultNode.childNode("blue")->appendChild(Text(""));
    }

    void 
    Histogram::configure(const dom::Node & theNode) {
    }

	void 
    Histogram::onFrame(dom::ValuePtr theRaster, double t) {
        std::vector<asl::Unsigned32> redHistogram(256);
        std::vector<asl::Unsigned32> blueHistogram(256);
        std::vector<asl::Unsigned32> greenHistogram(256);
        if (const RGBRaster * myFrame = dom::dynamic_cast_Value<RGBRaster>(&*theRaster)) {
            for (RGBRaster::const_iterator it = myFrame->begin(); it != myFrame->end(); ++it) {
                // AC_WARNING << static_cast<unsigned int>((*it)[0]);
                redHistogram[static_cast<unsigned int>((*it)[0])]++;
                greenHistogram[(*it)[1]]++;
                blueHistogram[(*it)[2]]++;
            }
        } else if (const BGRRaster * myFrame = dom::dynamic_cast_Value<BGRRaster>(&*theRaster)) {
            for (BGRRaster::const_iterator it = myFrame->begin(); it != myFrame->end(); ++it) {
                redHistogram[(*it)[2]]++;
                greenHistogram[(*it)[1]]++;
                blueHistogram[(*it)[0]]++;
            }
        }
        std::vector<asl::Unsigned64> mySum(3);
        mySum[0] = 0;
        mySum[1] = 0;
        mySum[2] = 0;
        for (int i = 0; i < 256; ++i) {
            mySum[0] += redHistogram[i]*i;
            mySum[1] += greenHistogram[i]*i;
            mySum[2] += blueHistogram[i]*i;
        }
        dom::Node & redNode = *(_myResultNode.childNode("red"));
        dom::Node & greenNode = *(_myResultNode.childNode("green"));
        dom::Node & blueNode = *(_myResultNode.childNode("blue"));

        //redNode.childNode("#text")->nodeValue(asl::as_string(redHistogram));
        redNode["sum"] = asl::as_string(mySum[0]);

        //greenNode.childNode("#text")->nodeValue(asl::as_string(greenHistogram));
        greenNode["sum"] = asl::as_string(mySum[1]);

        //blueNode.childNode("#text")->nodeValue(asl::as_string(blueHistogram));
        blueNode["sum"] = asl::as_string(mySum[2]);
	}
}
