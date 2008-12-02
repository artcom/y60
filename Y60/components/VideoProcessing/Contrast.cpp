//============================================================================
//
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

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
        for (unsigned char i=myUpper; i<256; i++) {
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
        
        const GRAYRaster * mySourceFrame = dom::dynamic_cast_Value<GRAYRaster>(&*_mySourceImage->getRasterValue());
        
        dom::ResizeableRasterPtr myResizeableRasterPtr = _mySourceImage->getRasterPtr();

        // // left-to-right horizontal pass
        GRAYRaster::iterator itSrc = const_cast<GRAYRaster::iterator>(mySourceFrame->begin());
        
        dom::Node::WritableValue<GRAYRaster> myTargetFrameLock(_myTargetImage->getRasterValueNode());
        GRAYRaster & myTargetFrame = myTargetFrameLock.get();
        GRAYRaster::iterator itTrgt = myTargetFrame.begin();
        
        for (itSrc; itSrc != mySourceFrame->end(); ++itSrc, ++itTrgt) {
            (*itTrgt) = _myLookupTable[(*itSrc).get()];
        }
	}
}

