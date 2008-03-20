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

#include "AdaptiveBackground.h"

using namespace asl;
using namespace std;
using namespace dom;

namespace y60 {

    AdaptiveBackground::AdaptiveBackground(const std::string & theName) :
        Algorithm(theName),
        _myResultNode("result"),
        _myCounter(98)
    {   
    }
  

    void 
    AdaptiveBackground::configure(const dom::Node & theNode) {
        
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
                } else if( myName == "blobimage") {
                    _myBlobImage = myImage->getFacade<y60::Image>();
                } 
            } else {
                if( myName == "strength") {
                    asl::fromString(myValue, _myStrength);
                }
            }
        }   
    }

	void 
    AdaptiveBackground::onFrame(double t) {
        if (_myCounter < 10) {
            _myCounter++;
            return;
        }
        
        _myCounter = 0;

        const BGRRaster * mySourceFrame = dom::dynamic_cast_Value<BGRRaster>(&*_mySourceImage->getRasterValue());
        const BGRRaster * myBlobFrame = dom::dynamic_cast_Value<BGRRaster>(&*_myBlobImage->getRasterValue());
        const BGRRaster * myTargetFrame = dom::dynamic_cast_Value<BGRRaster>(&*_myTargetImage->getRasterValue());
        
        BGRRaster::iterator itSrc   = const_cast<BGRRaster::iterator>(mySourceFrame->begin());
        BGRRaster::iterator itBlob  = const_cast<BGRRaster::iterator>(myBlobFrame->begin());
        BGRRaster::iterator itTarg  = const_cast<BGRRaster::iterator>(myTargetFrame->begin());

        for (itSrc; itSrc != mySourceFrame->end(); ++itSrc, ++itBlob, ++itTarg) {
            float myWeight = (static_cast<float>((*itBlob)[0]) / 256.0f);
            
            for (unsigned int i=0; i<3; ++i) {
                    (*itTarg)[i] = static_cast<unsigned char>((*itTarg)[i]) * myWeight;
                    (*itTarg)[i] += static_cast<unsigned char>((*itSrc)[i]) * (1-myWeight);
            }
        }

	}
}
