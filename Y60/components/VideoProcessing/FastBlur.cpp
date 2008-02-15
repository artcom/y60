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
            AC_PRINT << "configure " << myName;
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
        
        const GRAYRaster * mySourceFrame = dom::dynamic_cast_Value<GRAYRaster>(&*_mySourceImage->getRasterValue());
        
        dom::Node::WritableValue<GRAYRaster> myTargetFrameLock(_myTargetImage->getRasterValueNode());
        GRAYRaster & myTargetFrame = myTargetFrameLock.get();
        
        // float!
        GRAYRaster::const_iterator itSrc =mySourceFrame->begin();
        unsigned int i=0;

        for (itSrc; itSrc != mySourceFrame->end(); ++itSrc, ++i) {
            _myFloatImage[i] = static_cast<float>((*itSrc).get());
        }

        // left-to-right horizontal pass
        for (i=0; i<_myWidth*_myHeight; ++i) {
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
        GRAYRaster::iterator itTrgt = myTargetFrame.begin();
        
        i=0;
        for (itTrgt; itTrgt != myTargetFrame.end(); ++itTrgt, ++i) {
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
