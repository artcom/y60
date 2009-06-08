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

#include "ShotDetection.h"

namespace y60 {

    ShotDetectionAlgorithm::ShotDetectionAlgorithm(const std::string & theName) : 
    Algorithm( theName ),
    _myResultNode("shots"),
    _mySourceRaster(),
    _myThreshold(0.9),
    _myMinimalShotLength(1.0),
    _myLastShotTime(0)
    {
        clearHistogram(0);
        clearHistogram(1);
    }

void 
ShotDetectionAlgorithm::onFrame(double theTime) {
    const BGRRaster * myFrame = dom::dynamic_cast_Value<BGRRaster>(_mySourceRaster.get());
    static int n = 0;

    //clear current histogram
    clearHistogram(n);

    //compute histogram for current frame 
    int myScaled;
    int myFactors[] = {1, BINS_PER_CHANNEL, BINS_PER_CHANNEL*BINS_PER_CHANNEL };
    BGRRaster::const_iterator it;
    for (it = myFrame->begin(); it != myFrame->end(); ++it) {
        int myBin = 0;
        for (int c = 0; c < 3; ++c) {
            myScaled = (BINS_PER_CHANNEL * int((*it)[c])) / 256;
            myBin += myFactors[c] * myScaled;
        }

        /*
        AC_ERROR << myBin 
        << " color " << int((*it)[0]) << "," 
        << int((*it)[1]) << "," << int((*it)[2]);
        */
        _myHistogram[n][myBin]++;
    }

    if (_myResultNode.childNodesLength() == 0 && theTime > 0.5) {
        //wait some time before taking the first shot
        dom::Element myFirstShotNode("shot");
        myFirstShotNode["start"] = asl::as_string(0);
        addHistogram(n, myFirstShotNode);
        _myResultNode.appendChild(myFirstShotNode);
    }

    //printHistogram(n);

    //compute difference to histogram of last frame
    float myDifference = float(intersectHistograms())/myFrame->size();
    double myLength = theTime - _myLastShotTime;

    if (_myResultNode.childNodesLength() > 0) {
        //update length of current shot node
        dom::NodePtr myCurrentShot 
            = _myResultNode.childNode(_myResultNode.childNodesLength()-1);
        (*myCurrentShot)["length"] = asl::as_string(myLength);
    }

    AC_DEBUG << "************** diff at " << theTime << " is " << myDifference; 
    if (myDifference < _myThreshold) {
        //we have a potential shot

        if (myLength > _myMinimalShotLength) {

            //start a new shot node
            dom::Element myShotNode("shot");
            myShotNode["start"] = asl::as_string(theTime);
            myShotNode["length"] = asl::as_string(0);

            addHistogram(n, myShotNode);
            _myResultNode.appendChild(myShotNode);                

            AC_INFO << "SHOT at " << theTime << " last shot length " << myLength 
                << " (" << myDifference << ")";
        }
        _myLastShotTime = theTime;
    }

    //toggle current histogram
    n = 1 - n;
}

void 
ShotDetectionAlgorithm::configure(const dom::Node & theNode) { 
    try {
        AC_INFO << "configure scene " << _myScene; 
        for( unsigned int i=0; i<theNode.childNodesLength(); i++) {
            const std::string myName = theNode.childNode("property",0)->getAttribute("name")->nodeValue();
            const std::string myValue = theNode.childNode("property",0)->getAttribute("value")->nodeValue();
            AC_INFO << "configure " << myName << " " << myValue;
            if( myName == "sourceimage") {  
                dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(myValue);   
                if( myImage ) {
                    _mySourceRaster =  myImage->getFacade<y60::Image>()->getRasterValue();
                    //AC_INFO << "got source raster" << _mySourceRaster;
                }
            } else if( myName == "threshold" ) {
                asl::fromString(theNode["threshold"].nodeValue(), _myThreshold);    
            } else if( myName == "minimal_length" ) {
                asl::fromString(theNode["minimal_length"].nodeValue(), _myMinimalShotLength);   
            }   

        }
    } catch(asl::Exception ex) {
        AC_ERROR << "could not parse configuration " << theNode;
    }
    AC_INFO << "ShotDetection::configure sourceimage " << _mySourceRaster;
    AC_INFO << "ShotDetection::configure threshold " << _myThreshold;
    AC_INFO << "ShotDetection::configure min. shot length " << _myMinimalShotLength;
}

const dom::Node & 
ShotDetectionAlgorithm::result() const { 
    return _myResultNode;
}

void 
ShotDetectionAlgorithm::clearHistogram(int theIndex) {
    for(int b = 0; b < NBINS; ++b) {
        _myHistogram[theIndex][b] = 0;
    }
}

unsigned long 
ShotDetectionAlgorithm::intersectHistograms() {
    unsigned long myTotal = 0;
    for(int b = 0; b < NBINS; ++b) {
        myTotal += std::min(_myHistogram[0][b], _myHistogram[1][b]);
    }
    return myTotal;
}

//debug
void 
ShotDetectionAlgorithm::printHistogram(int theIndex) {
    for(int b = 0; b < NBINS; ++b) {
        std::cout << _myHistogram[theIndex][b] << " ";
    }
    std::cout << std::endl;
}

void 
ShotDetectionAlgorithm::addHistogram(int theIndex, dom::Node & theNode) {
    y60::VectorOfUnsignedInt myHistogram(_myHistogram[theIndex], 
        _myHistogram[theIndex] + NBINS);
    theNode("histogram").appendChild( dom::Text(asl::as_string(myHistogram)) );
}
}
