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

        unsigned int myWidth  = _mySourceImage->getRasterPtr()->width();
        unsigned int myHeight = _mySourceImage->getRasterPtr()->height();

        float myFloatImage[myWidth*myHeight];
        
        // float!
        BGRRaster::iterator itSrc = const_cast<BGRRaster::iterator>(mySourceFrame->begin());
        unsigned int i=0;

        for (itSrc; itSrc != mySourceFrame->end(); ++itSrc, ++i) {
            myFloatImage[i] = static_cast<float>((*itSrc)[0]);
        }


        // // left-to-right horizontal pass
        for (i=0; i<myWidth*myHeight; ++i) {
            myFloatImage[i] = revlookup(myFloatImage[i]) + lookup(myFloatImage[i-1]);
        }
       
        // right-to-left horizontal pass
        for (i=myWidth*myHeight-1; i>0; --i) {
            myFloatImage[i] = revlookup(myFloatImage[i]) + lookup(myFloatImage[i+1]);
        }
        
        // up-down vertical pass
        // itSrc = const_cast<BGRRaster::iterator>(mySourceFrame->begin());

        for (unsigned int w=0; w<myWidth; w++) {
            for (unsigned int h=1; h<myHeight; h++) {
                unsigned int pos0 = w+myWidth*(h-1);
                unsigned int pos1 = w+myWidth*h;

                myFloatImage[pos1] = revlookup(myFloatImage[pos1]) + lookup(myFloatImage[pos0]);
            }
        }

        // down-up vertical pass
        itSrc = const_cast<BGRRaster::iterator>(mySourceFrame->begin());

        for (unsigned int w=0; w<myWidth; w++) {
            for (unsigned int h=myHeight-1; h>0; h--) {
                unsigned int pos0 = w+myWidth*(h-1);
                unsigned int pos1 = w+myWidth*(h);
                
                myFloatImage[pos0] = revlookup(myFloatImage[pos0]) + lookup(myFloatImage[pos1]);
            }
        }


        // reverse float
        // float!
        itSrc = const_cast<BGRRaster::iterator>(mySourceFrame->begin());
       
        i=0;
        for (itSrc; itSrc != mySourceFrame->end(); ++itSrc, ++i) {
            for (unsigned int j=0; j<3; ++j) {
                (*itSrc)[j] = static_cast<unsigned char>(myFloatImage[i]);
            }
        }

        _myTargetImage->triggerUpload();
        
        // _myBackgroundImage->triggerUpload();
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
