#include "MaskPerson.h"
#include "cv_algo.h"
#include <asl/standard_pixel_types.h>

using namespace y60;
using namespace asl;
using namespace dom;


MaskPerson::MaskPerson( const std::string & theName ) : Algorithm(theName) {}

void MaskPerson::onFrame( double t ) {

    const raster<asl::BGR> * mySourceRaster = dynamic_cast_Value<raster<asl::BGR> >(&*_mySourceImage->getRasterValue());
    dom::Node::WritableValue<raster<asl::GRAY> > myTargetLock(_myTargetImage->getRasterValueNode());
    raster<asl::GRAY> & myTargetRaster = myTargetLock.get();

    raster<asl::GRAY> mySumRaster(mySourceRaster->hsize(), mySourceRaster->vsize());
    transform(mySourceRaster->begin(), mySourceRaster->end(), mySumRaster.begin(),
              channel_sum_function<asl::BGR, unsigned int>(0u));

    raster<asl::GRAY> myBlurRaster(mySourceRaster->hsize(), mySourceRaster->vsize());
    lowpass2d(_myBlurRadius, _myBlurRadius, mySumRaster, myBlurRaster, asl::gray<unsigned int>(0));

    discriminate(mySumRaster.begin(), mySumRaster.end(), myTargetRaster.begin(), 
                 asl::GRAY(_myThreshold), asl::GRAY(255), asl::GRAY(0)); 

}

void MaskPerson::configure( const dom::Node & theNode ) {

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
            }
        }
        if (myName == "threshold") {
            _myThreshold = as<unsigned int>(myValue);
        }
        if (myName == "blurradius") {
            _myBlurRadius = as<unsigned int>(myValue);
        }
    }
}
