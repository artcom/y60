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

using namespace asl;
using namespace std;
using namespace dom;

namespace y60 {

    BackgroundSubtraction::BackgroundSubtraction(const std::string & theName) :
        Algorithm(theName),
        _myResultNode("result"),
        _mySourceRaster(0)
    {   
        _myResultNode.appendChild(Element("center"));
        _myResultNode.childNode("center")->appendChild(Text(""));
        // _myResultNode.appendChild(Element("green"));
        // _myResultNode.childNode("green")->appendChild(Text(""));
        // _myResultNode.appendChild(Element("blue"));
        // _myResultNode.childNode("blue")->appendChild(Text(""));
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
        
        const BGRRaster * mySourceFrame     = dom::dynamic_cast_Value<BGRRaster>(&*_mySourceRaster);
        const BGRRaster * myBackgroundFrame = dom::dynamic_cast_Value<BGRRaster>(&*_myBackgroundRaster);
        const BGRRaster * myTargetFrame     = dom::dynamic_cast_Value<BGRRaster>(&*_myTargetRaster);
        
        BGRRaster::iterator itBg   = const_cast<BGRRaster::iterator>(myBackgroundFrame->begin());
        BGRRaster::iterator itTrgt = const_cast<BGRRaster::iterator>(myTargetFrame->begin());
            
        
        float myAlpha = _myWeight;
        unsigned int mySrcIntensity;
        unsigned int myBgIntensity;   
        unsigned int myTrgtIntensity;
        Vector3f mySrcHSV;
        Vector3f myBgHSV;
        for (BGRRaster::const_iterator itSrc = mySourceFrame->begin(); itSrc != mySourceFrame->end(); ++itSrc,++itBg, ++itTrgt) {
            rgb_to_intensity((*itSrc)[2], (*itSrc)[1], (*itSrc)[0], mySrcIntensity);
            rgb_to_intensity((*itTrgt)[2], (*itTrgt)[1], (*itTrgt)[0], myTrgtIntensity);
            rgb_to_intensity((*itBg)[2], (*itBg)[1], (*itBg)[0], myBgIntensity);
            
            //rgb_to_hsl((*itSrc)[2], (*itSrc)[1], (*itSrc)[0], mySrcHSV);
            //rgb_to_hsl((*itBg)[2], (*itBg)[1], (*itBg)[0], myBgHSV);

            myTrgtIntensity = clampedSub(myBgIntensity, mySrcIntensity);            
            //float myDiffSaturation = myBgHSV[1] - mySrcHSV[1];            
            //AC_PRINT << ;
            
            if( myTrgtIntensity > _myThreshold ) {
                //AC_PRINT << myHSV[0] << " " << myHSV[1] << " " << myHSV[2];
                (*itTrgt)[0] = 255;
                (*itTrgt)[1] = 255;
                (*itTrgt)[2] = 255;
            } else {
                (*itTrgt)[0] = 0;
                (*itTrgt)[1] = 0;
                (*itTrgt)[2] = 0;
            }
            
            // update backgroundimage
            // (*itBg)[0] = static_cast<unsigned int>(myAlpha * (*itSrc)[0] + (1-myAlpha)*(*itBg)[0]);
            // (*itBg)[1] = static_cast<unsigned int>(myAlpha * (*itSrc)[1] + (1-myAlpha)*(*itBg)[1]);
            // (*itBg)[2] = static_cast<unsigned int>(myAlpha * (*itSrc)[2] + (1-myAlpha)*(*itBg)[2]);

        }
        
        
        
//         BlobListPtr myBlobs = connectedComponents( _myTargetImage->getRasterPtr(), static_cast<int>(_myThreshold));
//         for(unsigned int blob = 0; blob < myBlobs->size(); ++blob) {
//             asl::Vector2f  myCenter =  (*myBlobs)[blob]->center();
//             _myTargetImage->getRasterPtr()->setPixel(asl::AC_SIZE_TYPE(myCenter[0]), asl::AC_SIZE_TYPE(myCenter[1]), Vector4f(0.0,1.0,0.0,1.0));
//             _myTargetImage->getRasterPtr()->setPixel(asl::AC_SIZE_TYPE(myCenter[0]+1), asl::AC_SIZE_TYPE(myCenter[1]), Vector4f(0.0,1.0,0.0,1.0));
//             _myTargetImage->getRasterPtr()->setPixel(asl::AC_SIZE_TYPE(myCenter[0]-1), asl::AC_SIZE_TYPE(myCenter[1]), Vector4f(0.0,1.0,0.0,1.0));
//             dom::Node & centerNode = *(_myResultNode.childNode("center"));
            
//             centerNode.childNode("#text")->nodeValue(asl::as_string(myCenter));
            
//             centerNode["x"] = asl::as_string(myCenter[0]);
//             centerNode["y"] = asl::as_string(myCenter[1]);
//             AC_PRINT << myCenter[0] << " " << myCenter[1];
//         }   
    
        _myTargetImage->triggerUpload();
        _myBackgroundImage->triggerUpload();
        
	}
    
    unsigned int
    BackgroundSubtraction::clampedSub(unsigned int theFirstValue, unsigned int theSecondValue) {
        return asl::maximum<unsigned int>(theFirstValue, theSecondValue) -  asl::minimum<unsigned int>(theFirstValue, theSecondValue);
    }

    float
    BackgroundSubtraction::clampedSubHSL(unsigned int theFirstValue, unsigned int theSecondValue) {
        return asl::maximum<float>(theFirstValue, theSecondValue) -  asl::minimum<float>(theFirstValue, theSecondValue);
    }
}
