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
        _mySourceRaster(0),
        _myCounter(0)
    {   
    }
  

    void 
    BackgroundSubtraction::configure(const dom::Node & theNode) {
        
        for( unsigned int i=0; i<theNode.childNodesLength(); i++) {
            const std::string myName = theNode.childNode("property",i)->getAttribute("name")->nodeValue();
            const std::string myValue = theNode.childNode("property",i)->getAttribute("value")->nodeValue();
            dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(myValue);
            AC_PRINT << "configure " << myName;
            if( myImage ) {
                if( myName == "sourceimage") {
                    _mySourceRaster = myImage->getFacade<y60::Image>()->getRasterValue();
                } else if( myName == "backgroundimage") {
                    _myBackgroundRaster = myImage->getFacade<y60::Image>()->getRasterValue();
                    _myBackgroundImage = myImage->getFacade<y60::Image>();
                    AC_PRINT << "raster " << _myBackgroundRaster;
                } else if( myName == "targetimage") {
                    _myTargetRaster =  myImage->getFacade<y60::Image>()->getRasterValue();
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
        
        const GRAYRaster * mySourceFrame     = dom::dynamic_cast_Value<GRAYRaster>(&*_mySourceRaster);
        const GRAYRaster * myBackgroundFrame = dom::dynamic_cast_Value<GRAYRaster>(&*_myBackgroundRaster);
        const GRAYRaster * myTargetFrame     = dom::dynamic_cast_Value<GRAYRaster>(&*_myTargetRaster);
        
        GRAYRaster::iterator itBg   = const_cast<GRAYRaster::iterator>(myBackgroundFrame->begin());
        GRAYRaster::iterator itTrgt = const_cast<GRAYRaster::iterator>(myTargetFrame->begin());
        
        
        float myAlpha = _myWeight;
        asl::gray<unsigned char> mySrcIntensity;
        asl::gray<unsigned char> myBgIntensity;   
        asl::gray<unsigned char> myTrgtIntensity;
        
        for (GRAYRaster::const_iterator itSrc = mySourceFrame->begin(); itSrc != mySourceFrame->end(); ++itSrc,++itBg, ++itTrgt) {


            myTrgtIntensity = clampedSub((*itBg).get(), (*itSrc).get());            
            //AC_PRINT << myTrgtIntensity;            
            (*itTrgt) = myTrgtIntensity;
            
            //"adaptive" background
        //     if (_myCounter >= COUNTER__) {
        //         (*itBg) = myAlpha * (*itSrc).get() + (1-myAlpha)*(*itBg).get();
        //     }
        
            (*itBg) = (*itSrc).get();
        }
        
        // update backgroundimage
        // if (_myCounter >= COUNTER__) {
        //     _myCounter = 0;
        // }
        
        _myCounter++;
        
        
	}
    
    unsigned char
    BackgroundSubtraction::clampedSub(unsigned char theFirstValue, unsigned char theSecondValue) {
        return asl::maximum<unsigned char>(theFirstValue, theSecondValue) -  asl::minimum<unsigned char>(theFirstValue, theSecondValue);
    }
}
