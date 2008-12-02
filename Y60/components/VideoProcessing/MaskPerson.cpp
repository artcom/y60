#include "MaskPerson.h"
#include "cv_algo.h"
#include <asl/raster/standard_pixel_types.h>

using namespace y60;
using namespace asl;
using namespace dom;


MaskPerson::MaskPerson( const std::string & theName ) : Algorithm(theName) {}

void MaskPerson::onFrame( double t ) {

    const raster<asl::BGR> * mySourceRaster = dynamic_cast_Value<raster<asl::BGR> >(&*_mySourceImage->getRasterValue());
    dom::Node::WritableValue<raster<asl::BGR> > myTargetLock(_myTargetImage->getRasterValueNode());
    raster<asl::BGR> & myTargetRaster = myTargetLock.get();
    dom::Node::WritableValue<raster<asl::GRAY> > myMaskLock(_myTargetMask->getRasterValueNode());
    raster<asl::GRAY> & myMaskRaster = myMaskLock.get();

    raster<asl::GRAY> mySumRaster(mySourceRaster->hsize(), mySourceRaster->vsize());
    transform(mySourceRaster->begin(), mySourceRaster->end(), mySumRaster.begin(),
              channel_sum_function<asl::BGR, unsigned int>(0u));

    raster<asl::GRAY> myBlurRaster(mySourceRaster->hsize(), mySourceRaster->vsize());
    lowpass2d(3, 3, mySumRaster, myBlurRaster, asl::gray<unsigned int>(0));

    raster<asl::GRAY> myBinaryRaster(mySourceRaster->hsize(), mySourceRaster->vsize());
    discriminate(myBlurRaster.begin(), myBlurRaster.end(), myBinaryRaster.begin(), 
        asl::GRAY(static_cast<asl::pchar>(_myThreshold)), asl::GRAY(255), asl::GRAY(0)); 

    std::copy(myBinaryRaster.begin(), myBinaryRaster.end(), myMaskRaster.begin());
    
//    raster<asl::GRAY> myBluredBinary(mySourceRaster->hsize(), mySourceRaster->vsize());
//    lowpass2d(_myBlurRadius, _myBlurRadius, myBinaryRaster, myBluredBinary, 
//              asl::gray<unsigned int>(0));

    raster<asl::gray<int> > myHGradient(mySourceRaster->hsize(), mySourceRaster->vsize());
    h_gradient(_myGradientRadius, myBinaryRaster, myHGradient, asl::gray<int>(0)); 

    raster<asl::gray<int> > myVGradient(mySourceRaster->hsize(), mySourceRaster->vsize());
    v_gradient(_myGradientRadius, myBinaryRaster, myVGradient, asl::gray<int>(0)); 

    raster<asl::BGR>::iterator itTrgt = myTargetRaster.begin();
    raster<asl::gray<int> >::iterator itHGrad = myHGradient.begin();    
    raster<asl::gray<int> >::iterator itVGrad = myVGradient.begin();    
    raster<asl::GRAY>::iterator itBlur = myBinaryRaster.begin();//myBluredBinary.begin();     
    for (;   
         itTrgt != myTargetRaster.end(); 
         ++itTrgt, ++itHGrad, ++itVGrad, ++itBlur)
    {
        (*itTrgt) = asl::BGR( static_cast<asl::pchar>((itHGrad->get() + 255)/2)
                            , static_cast<asl::pchar>((itVGrad->get() + 255)/2)
                            , itBlur->get());
    }
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
            } else if (myName == "targetmask") {
                _myTargetMask = myImage->getFacade<y60::Image>();
            }
        }
        if (myName == "threshold") {
            _myThreshold = as<unsigned int>(myValue);
        }
        if (myName == "blurradius") {
            _myBlurRadius = as<unsigned int>(myValue);
        }
        if (myName == "gradientradius") {
            _myGradientRadius = as<unsigned int>(myValue);
        }
    }
}
