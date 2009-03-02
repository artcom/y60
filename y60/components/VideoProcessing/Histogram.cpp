/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Histogram.h"

#include <y60/jslib/AbstractRenderWindow.h>

using namespace asl;
using namespace std;
using namespace dom;

namespace y60 {

    Histogram::Histogram(const std::string theName) : 
        Algorithm(theName),
        _myResultNode("result"), 
        _mySourceRaster()
    {
        _myResultNode.appendChild(Element("red"));
        _myResultNode.childNode("red")->appendChild(dom::Text(""));
        _myResultNode.appendChild(Element("green"));
        _myResultNode.childNode("green")->appendChild(dom::Text(""));
        _myResultNode.appendChild(Element("blue"));
        _myResultNode.childNode("blue")->appendChild(dom::Text(""));
    }

    void 
    Histogram::configure(const dom::Node & theNode) {
        for( unsigned int i=0; i<theNode.childNodesLength(); i++) {
            const std::string myName = theNode.childNode("property",0)->getAttribute("name")->nodeValue();
            const std::string myID = theNode.childNode("property",0)->getAttribute("value")->nodeValue();
            dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(myID);
            if( myImage ) {
                if( myName == "sourceimage") {
                    _mySourceRaster =  myImage->getFacade<y60::Image>()->getRasterValue();
                    //const BGRRaster * myFrame = dom::dynamic_cast_Value<BGRRaster>(_mySourceRaster.get());
                    //AC_INFO << "histogram configure " << *myFrame;
                }
            }   
        }       

    }

	void 
    Histogram::onFrame(double t) {
        std::vector<asl::Unsigned32> redHistogram(256);
        std::vector<asl::Unsigned32> blueHistogram(256);
        std::vector<asl::Unsigned32> greenHistogram(256);
        if (const RGBRaster * myFrame = dom::dynamic_cast_Value<RGBRaster>(_mySourceRaster.get())) {
            for (RGBRaster::const_iterator it = myFrame->begin(); it != myFrame->end(); ++it) {
                // AC_WARNING << static_cast<unsigned int>((*it)[0]);
                redHistogram[static_cast<unsigned int>((*it)[0])]++;
                greenHistogram[(*it)[1]]++;
                blueHistogram[(*it)[2]]++;
            }

        } else if (const BGRRaster * myFrame = dom::dynamic_cast_Value<BGRRaster>(_mySourceRaster.get())) {
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
