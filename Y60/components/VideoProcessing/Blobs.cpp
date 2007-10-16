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

#include "Blobs.h"
#include "ConnectedComponent.h"

using namespace asl;
using namespace std;
using namespace dom;

namespace y60 {

    Blobs::Blobs(const std::string & theName) :
        Algorithm(theName),
        _myResultNode("result"),
        _myThreshold(5)
    {   
    }
  

    void 
    Blobs::configure(const dom::Node & theNode) {
        
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
                if( myName == "threshold") {
                    asl::fromString(myValue, _myThreshold);
                    AC_ERROR << "setting threshold: " << _myThreshold;
                }
            }
        }   
    }

	void 
    Blobs::onFrame(double t) {
        y60::ImagePtr myGrayImage = _mySourceImage;

        const BGRRaster * mySourceFrame = dom::dynamic_cast_Value<BGRRaster>(&*_mySourceImage->getRasterValue());
        const GRAYRaster * myTargetFrame = dom::dynamic_cast_Value<GRAYRaster>(&*_myTargetImage->getRasterValue());
        
        unsigned int mySrcIntensity;
        unsigned int myBgIntensity;   
        unsigned int myTrgtIntensity;
        Vector3f mySrcHSV;
        Vector3f myBgHSV;

        dom::ResizeableRasterPtr myResizeableRasterPtr = _mySourceImage->getRasterPtr();

        // // left-to-right horizontal pass
        BGRRaster::iterator itSrc = const_cast<BGRRaster::iterator>(mySourceFrame->begin());
        GRAYRaster::iterator itTarget = const_cast<GRAYRaster::iterator>(myTargetFrame->begin());
        
        for (itSrc; itSrc != mySourceFrame->end(); ++itSrc, ++itTarget) {
            (*itTarget) = (*itSrc)[0];
        }
        
        BlobListPtr myBlobs = connectedComponents( _myTargetImage->getRasterPtr(), static_cast<int>(_myThreshold));

        for (unsigned int blob = 0; blob < myBlobs->size(); ++blob) {
            asl::Vector2f  myCenter =  (*myBlobs)[blob]->center();
            _mySourceImage->getRasterPtr()->setPixel(asl::AC_SIZE_TYPE(myCenter[0]+1), asl::AC_SIZE_TYPE(myCenter[1]), Vector4f(0.0,1.0,0.0,1.0));
            _mySourceImage->getRasterPtr()->setPixel(asl::AC_SIZE_TYPE(myCenter[0]),   asl::AC_SIZE_TYPE(myCenter[1]), Vector4f(1.0,0.0,0.0,1.0));
            _mySourceImage->getRasterPtr()->setPixel(asl::AC_SIZE_TYPE(myCenter[0]-1), asl::AC_SIZE_TYPE(myCenter[1]), Vector4f(0.0,1.0,0.0,1.0));
            
            // dom::Node & centerNode = *(_myResultNode.childNode("center"));
            // centerNode.childNode("#text")->nodeValue(asl::as_string(myCenter));
         
            // centerNode["x"] = asl::as_string(myCenter[0]);
            // centerNode["y"] = asl::as_string(myCenter[1]);
            // AC_PRINT << "no: " << blob << " - "<< myCenter[0] << " " << myCenter[1];
        }   

        _mySourceImage->triggerUpload();
	}
}
