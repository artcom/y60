#include "Canny.h"

using namespace asl;
using namespace dom;
using namespace std;
using namespace y60;


Canny::Canny( const string & theName ) : Algorithm( theName ) {
}

void Canny::onFrame( double t ) {

    const GRAYRaster * mySourceFrame = 
        dom::dynamic_cast_Value<GRAYRaster>(&*_mySourceImage->getRasterValue());
    dom::Node::WritableValue<GRAYRaster> myTargetFrameLock(_myTargetImage->getRasterValueNode());
    GRAYRaster & myTargetFrame = myTargetFrameLock.get();

    float mySobelX[9] = { -1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0 };
    float mySobelY[9] = { -1.0, -2.0, -1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0 };

    GRAYRaster::iterator itTrgt = myTargetFrame.begin();

    for (unsigned y = 0; y < _myHeight; y++) {
        for (unsigned x = 0; x < _myWidth; x++) {
            GRAYRaster::const_sub_iterator itSrc = mySourceFrame->begin(x,y,3,3);
            unsigned i = 0;
            float gx = 0;
            float gy = 0;
            while (itSrc != mySourceFrame->end(x,y,3,3)) {
                float mySrcFloat = static_cast<float>((*itSrc++).get());
                gx += mySobelX[i++] * mySrcFloat;
                gy += mySobelY[i++] * mySrcFloat;
            }
            (*itTrgt++) = static_cast<unsigned char>(sqrt(gx*gx + gy*gy));            
        }
    }
}


void Canny::configure( const dom::Node & theNode ) {

    for (unsigned i = 0; i < theNode.childNodesLength(); i++) {
        const std::string myName = 
            theNode.childNode("property",i)->getAttribute("name")->nodeValue();
        const std::string myValue = 
            theNode.childNode("property",i)->getAttribute("value")->nodeValue();

        dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(myValue);
        if (myImage) {
            if( myName == "sourceimage") {
                _mySourceImage = myImage->getFacade<y60::Image>();
            } else if( myName == "targetimage") {
                _myTargetImage = myImage->getFacade<y60::Image>();
                _myWidth  = _mySourceImage->getRasterPtr()->width();
                _myHeight = _mySourceImage->getRasterPtr()->height();
            } 
        }
    }
}
