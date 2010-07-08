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

#include "DumbScale.h"

using namespace asl;
using namespace std;
using namespace dom;

namespace y60 {

    DumbScale::DumbScale(const std::string & theName) :
        Algorithm(theName),
        _myResultNode("result"),
        _mySourceRaster(),
        _myTargetRaster(),
        _myImageNodeVersion(0)
    {
    }

    void
    DumbScale::configure(const dom::Node & theNode) {

        for( unsigned int i=0; i<theNode.childNodesLength(); i++) {
            const std::string myName = theNode.childNode("property",i)->getAttribute("name")->nodeValue();
            const std::string myValue = theNode.childNode("property",i)->getAttribute("value")->nodeValue();
            dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(myValue);
            AC_INFO << "configure " << myName;
            if( myImage ) {
                if( myName == "sourceimage") {
                    _mySourceImage = myImage->getFacade<y60::Image>();
                    _mySourceRaster = _mySourceImage->getRasterValue();
                } else if( myName == "targetimage") {
                    _myTargetRaster =  myImage->getFacade<y60::Image>()->getRasterValue();
                    _myTargetImage = myImage->getFacade<y60::Image>();
                }
            } else {
                if( myName == "width") {
                    asl::fromString(myValue, _myWidth);
                } else if( myName == "height") {
                    asl::fromString(myValue, _myHeight);
                }
            }
        }
    }

    void
    DumbScale::onFrame(double t) {

        asl::Unsigned64 myImageNodeVersion = _mySourceImage->getRasterValueNode()->nodeVersion();
        if (myImageNodeVersion > _myImageNodeVersion) {
            _myImageNodeVersion = myImageNodeVersion;
        } else {
            return;
        }
#define ORIG
#ifdef ORIG
        const BGRRaster * mySourceFrame  = dom::dynamic_cast_Value<BGRRaster>(_mySourceRaster.get());
        dom::Node::WritableValue<GRAYRaster> myTargetLock(_myTargetImage->getRasterValueNode());
        GRAYRaster & myTargetFrame = myTargetLock.get();

        GRAYRaster::iterator itTrgt = myTargetFrame.begin();

        unsigned int myLineSubCounter = 0;

        unsigned int myDebugCount = 0;
        bool myLineFlag = true;

        for (BGRRaster::const_iterator itSrc = mySourceFrame->begin();
             itSrc != mySourceFrame->end();
             itSrc+=2)
        {

            // get only every second pixel in a line
            myLineSubCounter++;
            if (myLineSubCounter == (_myWidth/2)) {
                myLineSubCounter=0;

                myLineFlag = !myLineFlag;
            }

            if (myLineFlag) {
                unsigned char myTarget = (*itTrgt).get();
                rgb_to_intensity((*itSrc)[2], (*itSrc)[1], (*itSrc)[0], myTarget);
                (*itTrgt++) = myTarget;
                myDebugCount++;
            }
        }
#else
    // TODO: make proper raster functionality here
#endif
	}
}
