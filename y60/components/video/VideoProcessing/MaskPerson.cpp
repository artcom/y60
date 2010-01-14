/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "MaskPerson.h"
#include "cv_algo.h"
#include <asl/raster/standard_pixel_types.h>

using namespace y60;
using namespace asl;
using namespace dom;


MaskPerson::MaskPerson( const std::string & theName ) : Algorithm(theName) {}

void MaskPerson::onFrame( double t ) {

    const raster<asl::BGR> * mySourceRaster = dynamic_cast_Value<raster<asl::BGR> >(_mySourceImage->getRasterValue().get());
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
