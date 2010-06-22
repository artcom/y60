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

#include "FastBlur.h"

using namespace asl;
using namespace std;
using namespace dom;

namespace y60 {

    FastBlur::FastBlur(const std::string & theName) :
        Algorithm(theName),
        _myResultNode("result"),
        _myImageNodeVersion(0)
    {
    }

    FastBlur::~FastBlur()
    {
        delete [] _myFloatImage;
    }


    void
    FastBlur::configure(const dom::Node & theNode) {

        for( unsigned int i=0; i<theNode.childNodesLength(); i++) {
            const std::string myName = theNode.childNode("property",i)->getAttribute("name")->nodeValue();
            const std::string myValue = theNode.childNode("property",i)->getAttribute("value")->nodeValue();
            dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(myValue);
            AC_INFO << "configure " << myName;
            if( myImage ) {
                if( myName == "sourceimage") {
                    _mySourceImage = myImage->getFacade<y60::Image>();
                    _myWidth  = _mySourceImage->getRasterPtr()->width();
                    _myHeight = _mySourceImage->getRasterPtr()->height();

                    _myFloatImage = new float[_myWidth*_myHeight];

                } else if( myName == "targetimage") {
                    _myTargetImage = myImage->getFacade<y60::Image>();
                }
            } else {
                if( myName == "strength") {
                    asl::fromString(myValue, _myStrength);
                }
            }
        }
    }

    void
    FastBlur::onFrame(double t) {
        asl::Unsigned64 myImageNodeVersion = _mySourceImage->getRasterValueNode()->nodeVersion();
        if (myImageNodeVersion > _myImageNodeVersion) {
            _myImageNodeVersion = myImageNodeVersion;
        } else {
            return;
        }

        const GRAYRaster * mySourceFrame = dom::dynamic_cast_Value<GRAYRaster>(_mySourceImage->getRasterValue().get());

        // float!
        GRAYRaster::const_iterator itSrc = mySourceFrame->begin();
        unsigned int i=0;

        for (; itSrc != mySourceFrame->end(); ++itSrc, ++i) {
            _myFloatImage[i] = static_cast<float>((*itSrc).get());
        }

        // left-to-right horizontal pass
        for (i=1; i<_myWidth*_myHeight; ++i) {
            _myFloatImage[i] = revlookup(_myFloatImage[i]) + lookup(_myFloatImage[i-1]);
        }

        // right-to-left horizontal pass
        for (i=_myWidth*_myHeight-1; i>1; --i) {
            _myFloatImage[i-1] = revlookup(_myFloatImage[i-1]) + lookup(_myFloatImage[i]);
        }

        // up-down vertical pass
        for (unsigned int w=0; w<_myWidth; w++) {
            for (unsigned int h=1; h<_myHeight; h++) {
                unsigned int pos0 = w+_myWidth*(h-1);
                unsigned int pos1 = w+_myWidth*h;

                _myFloatImage[pos1] = revlookup(_myFloatImage[pos1]) + lookup(_myFloatImage[pos0]);
            }
        }

        // down-up vertical pass
        for (unsigned int w=0; w<_myWidth; w++) {
            for (unsigned int h=_myHeight-1; h>0; h--) {
                unsigned int pos0 = w+_myWidth*(h-1);
                unsigned int pos1 = w+_myWidth*(h);

                _myFloatImage[pos0] = revlookup(_myFloatImage[pos0]) + lookup(_myFloatImage[pos1]);
            }
        }

        // reverse float
        // float!
        dom::Node::WritableValue<GRAYRaster> myTargetFrameLock(_myTargetImage->getRasterValueNode());
        GRAYRaster & myTargetFrame = myTargetFrameLock.get();
        GRAYRaster::iterator itTrgt = myTargetFrame.begin();

        i=0;
        for (; itTrgt != myTargetFrame.end(); ++itTrgt, ++i) {
            (*itTrgt) = static_cast<unsigned char>(_myFloatImage[i]);
        }
	}

    float
    FastBlur::lookup(float theValue) {
        return (float) (theValue * _myStrength);
    }

    float
    FastBlur::revlookup(float theValue) {
        return (float) (theValue * (1.0f - _myStrength));
    }
}
