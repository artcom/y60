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

#include "Contrast.h"

using namespace asl;
using namespace std;
using namespace dom;

namespace y60 {

    Contrast::Contrast(const std::string & theName) :
        Algorithm(theName),
        _myResultNode("result"),
        _myLower(0),
        _myUpper(255),
        _myImageNodeVersion(0)
    {
    }

    void
    Contrast::configure(const dom::Node & theNode) {

        for( unsigned int i=0; i<theNode.childNodesLength(); i++) {
            const std::string myName = theNode.childNode("property",i)->getAttribute("name")->nodeValue();
            const std::string myValue = theNode.childNode("property",i)->getAttribute("value")->nodeValue();
            dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(myValue);
            AC_INFO << "configure " << myName;
            if( myImage ) {
                if( myName == "sourceimage") {
                    _mySourceImage = myImage->getFacade<y60::Image>();
                } else if( myName == "targetimage") {
                    _myTargetImage = myImage->getFacade<y60::Image>();
                }
            } else {
                if( myName == "lower") {
                    asl::fromString(myValue, _myLower);
                    updateLookupTable();
                } else if( myName == "upper") {
                    asl::fromString(myValue, _myUpper);
                    updateLookupTable();
                }
            }
        }
    }

    void
    Contrast::updateLookupTable() {
        _myLookupTable.clear();

        const unsigned char myUpper = static_cast<unsigned char>(_myUpper);
        const unsigned char myLower = static_cast<unsigned char>(_myLower);

        unsigned char myRange = static_cast<asl::pchar>(myUpper - myLower);

        for (unsigned char i=0; i<_myLower; i++) {
            _myLookupTable.push_back(0);
        }
        for (unsigned char i=myLower, value = 1; i<myUpper; i++, value++) {
            unsigned char myValue = value * 255 / myRange;
            _myLookupTable.push_back(myValue);
        }
        for (unsigned int i=myUpper; i<=255; i++) {
            _myLookupTable.push_back(255);
        }
    }

    void
    Contrast::onFrame(double t) {
        asl::Unsigned64 myImageNodeVersion = _mySourceImage->getRasterValueNode()->nodeVersion();
        if (myImageNodeVersion > _myImageNodeVersion) {
            _myImageNodeVersion = myImageNodeVersion;
        } else {
            return;
        }

        const GRAYRaster * mySourceFrame = dom::dynamic_cast_Value<GRAYRaster>(_mySourceImage->getRasterValue().get());

        dom::ResizeableRasterPtr myResizeableRasterPtr = _mySourceImage->getRasterPtr();

        // // left-to-right horizontal pass
        GRAYRaster::iterator itSrc = const_cast<GRAYRaster::iterator>(mySourceFrame->begin());

        dom::Node::WritableValue<GRAYRaster> myTargetFrameLock(_myTargetImage->getRasterValueNode());
        GRAYRaster & myTargetFrame = myTargetFrameLock.get();
        GRAYRaster::iterator itTrgt = myTargetFrame.begin();

        for (; itSrc != mySourceFrame->end(); ++itSrc, ++itTrgt) {
            (*itTrgt) = _myLookupTable[(*itSrc).get()];
        }
	}
}

