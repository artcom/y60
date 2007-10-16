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

#include "FastBlur.h"

using namespace asl;
using namespace std;
using namespace dom;

namespace y60 {

    FastBlur::FastBlur(const std::string & theName) :
        Algorithm(theName),
        _myResultNode("result")
    {   
    }
  

    void 
    FastBlur::configure(const dom::Node & theNode) {
        
        for( unsigned int i=0; i<theNode.childNodesLength(); i++) {
            const std::string myName = theNode.childNode("property",i)->getAttribute("name")->nodeValue();
            const std::string myValue = theNode.childNode("property",i)->getAttribute("value")->nodeValue();
            dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(myValue);
            AC_PRINT << "configure " << myName;
            if( myImage ) {
                if( myName == "sourceimage") {
                    _mySourceImage = myImage->getFacade<y60::Image>();
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
        
        const BGRRaster * mySourceFrame = dom::dynamic_cast_Value<BGRRaster>(&*_mySourceImage->getRasterValue());
        const BGRRaster * myTargetFrame = dom::dynamic_cast_Value<BGRRaster>(&*_myTargetImage->getRasterValue());
        
        // BGRRaster::iterator itSrc  = const_cast<BGRRaster::iterator>(mySourceFrame->begin());
        // BGRRaster::iterator itTrgt = const_cast<BGRRaster::iterator>(myTargetFrame->begin());
        
        unsigned int mySrcIntensity;
        unsigned int myBgIntensity;   
        unsigned int myTrgtIntensity;
        Vector3f mySrcHSV;
        Vector3f myBgHSV;

        dom::ResizeableRasterPtr myResizeableRasterPtr = _mySourceImage->getRasterPtr();

        /* for (unsigned int w=1; w<myResizeableRasterPtr->width(); ++w) {
            for (unsigned int h=0; h<myResizeableRasterPtr->height(); ++h) {
                asl::Vector4f myColor0 = myResizeableRasterPtr->getPixel(w-1, h);
                asl::Vector4f myColor1 = myResizeableRasterPtr->getPixel(w, h);
                // AC_PRINT << myColor1;

                myResizeableRasterPtr->setPixel(w, h, weightedAdd(myColor0, myColor1));
            }
            //      _mySourceImage->getRasterPtr()->setPixel(asl::AC_SIZE_TYPE(myCenter[0]), asl::AC_SIZE_TYPE(myCenter[1]), Vector4f(0.0,1.0,0.0,1.0));
        } */

        // // left-to-right horizontal pass
        BGRRaster::iterator itSrc = const_cast<BGRRaster::iterator>(mySourceFrame->begin());
        itSrc++;

        for (itSrc; itSrc != mySourceFrame->end(); ++itSrc) {
            for (unsigned int i=0; i<3; i++) {
                (*itSrc)[i] = revlookup((*itSrc)[i]) + lookup((*(itSrc-1))[i]);
            }
        }
        
        itSrc = const_cast<BGRRaster::iterator>(mySourceFrame->end());
        itSrc--;
        for (itSrc; itSrc != mySourceFrame->begin(); --itSrc) {
            for (unsigned int i=0; i<3; i++) {
                (*itSrc)[i] = revlookup((*itSrc)[i]) + lookup((*(itSrc+1))[i]);
            }
        }
        
        _myTargetImage->triggerUpload();
        
        // _myBackgroundImage->triggerUpload();
	}

    unsigned char
    FastBlur::lookup(unsigned char theValue) {
        return (unsigned char) (theValue * _myStrength);
    }
    
    unsigned char
    FastBlur::revlookup(unsigned char theValue) {
        return (unsigned char) (theValue * (1.0f - _myStrength));
    }
}
