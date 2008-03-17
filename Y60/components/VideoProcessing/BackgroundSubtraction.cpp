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

#include "BackgroundSubtraction.h"

//include "ConnectedComponent.h"

#define COUNTER__ 1

using namespace asl;
using namespace std;
using namespace dom;

namespace y60 {

    BackgroundSubtraction::BackgroundSubtraction(const std::string & theName) :
        Algorithm(theName),
        _myResultNode("result"),
        _myCounter(0),
        _myImageNodeVersion(0)
    {   
    }
  

    void 
    BackgroundSubtraction::configure(const dom::Node & theNode) {
        
        for( unsigned int i=0; i<theNode.childNodesLength(); i++) {
            const std::string myName = 
                theNode.childNode("property",i)->getAttribute("name")->nodeValue();
            const std::string myValue = 
                theNode.childNode("property",i)->getAttribute("value")->nodeValue();
            dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(myValue);

            //AC_PRINT << "configure " << myName;

            if( myImage ) {
                if( myName == "sourceimage") {
                    _mySourceImage = myImage->getFacade<y60::Image>();
                } else if( myName == "backgroundimage") {
                    _myBackgroundImage = myImage->getFacade<y60::Image>();
                } else if( myName == "targetimage") {
                    _myTargetImage = myImage->getFacade<y60::Image>();
                } 
            } else {
                if( myName == "threshold") {
                    asl::fromString(myValue, _myThreshold);
                } else if( myName == "weight") {
                    asl::fromString(myValue, _myWeight);
                }

            }
        }   
        // TODO:  add colorspace and dimension checking! [sh]
    }

	void 
    BackgroundSubtraction::onFrame(double t) {
        
        unsigned myImageNodeVersion = _mySourceImage->getRasterValueNode()->nodeVersion(); 
        if (myImageNodeVersion > _myImageNodeVersion) {
            _myImageNodeVersion = myImageNodeVersion;
        } else {
            return;
        }

        const GRAYRaster & mySourceFrame =  
            _mySourceImage->getRasterValueNode()->nodeValueRef<GRAYRaster>();
        dom::Node::WritableValue<GRAYRaster> 
            myBackgroundLock(_myBackgroundImage->getRasterValueNode());
        dom::Node::WritableValue<GRAYRaster>
            myTargetLock(_myTargetImage->getRasterValueNode());
      
        GRAYRaster & myBackgroundFrame = myBackgroundLock.get();
        GRAYRaster::iterator itBg   = myBackgroundFrame.begin();

        GRAYRaster & myTargetFrame = myTargetLock.get();
        GRAYRaster::iterator itTrgt = myTargetFrame.begin();
        
        float myAlpha = _myWeight;
        asl::gray<unsigned char> mySrcIntensity;
        asl::gray<unsigned char> myBgIntensity;   
        asl::gray<unsigned char> myTrgtIntensity;
        
        for (GRAYRaster::const_iterator itSrc = mySourceFrame.begin(); 
             itSrc != mySourceFrame.end(); 
             ++itSrc,++itBg, ++itTrgt) 
        {
        #if 1 // motion difference
            myTrgtIntensity = clampedSub((*itBg).get(), (*itSrc).get());  
            //AC_PRINT << myTrgtIntensity;            
            (*itTrgt) = myTrgtIntensity;
        #else    
            //"adaptive" background
            if (_myCounter >= COUNTER__) {
                (*itBg) = myAlpha * (*itSrc).get() + (1-myAlpha)*(*itBg).get();
            }
        #endif
            (*itBg) = (*itSrc).get();
        }
	}
    
    unsigned char
    BackgroundSubtraction::clampedSub(unsigned char theFirstValue, unsigned char theSecondValue) {
        return asl::maximum<unsigned char>(theFirstValue, theSecondValue) -  asl::minimum<unsigned char>(theFirstValue, theSecondValue);
    }
}
