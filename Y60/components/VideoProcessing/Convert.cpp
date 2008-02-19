#include "Convert.h"

using namespace dom;
using namespace asl;
using namespace y60;
using namespace std;

void Convert::configure( const Node & theNode ) {

    for (unsigned int i = 0; i < theNode.childNodesLength(); i++) {

        const string myName = theNode.childNode("property", i)->getAttribute("name")->nodeValue();
        const string myValue = theNode.childNode("property", i)->getAttribute("value")->nodeValue();
        dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(myValue);
        if (myImage) {
            if (myName == "sourceimage") {
                _mySourceImage = myImage->getFacade<y60::Image>();
            } else if (myName == "targetimage") {
                _myTargetImage = myImage->getFacade<y60::Image>();
            }
        }
    }
}


void Convert::onFrame( double t ) {

    
    const BGRRaster * mySourceFrame = 
        dynamic_cast_Value<BGRRaster>( &*(_mySourceImage->getRasterValue()) );
    dom::Node::WritableValue<GRAYRaster> myTargetFrameLock(_myTargetImage->getRasterValueNode());
    GRAYRaster & myTargetFrame = myTargetFrameLock.get();

    GRAYRaster::iterator itTrgt = myTargetFrame.begin();
    for (BGRRaster::const_iterator itSrc = mySourceFrame->begin(); 
         itSrc != mySourceFrame->end(); 
         itSrc++) 
    {

        unsigned char myTarget = (*itTrgt).get();
        rgb_to_intensity((*itSrc)[2], (*itSrc)[1], (*itSrc)[0], myTarget);
        (*itTrgt++).set(myTarget);
    }
}
