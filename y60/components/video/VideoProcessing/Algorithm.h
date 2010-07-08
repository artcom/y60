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

#ifndef _ac_VideoProcessing_Algorithm_h_
#define _ac_VideoProcessing_Algorithm_h_

#include "y60_videoprocessing_settings.h"

#include <y60/image/Image.h>
#include <y60/scene/Scene.h>

namespace y60 {

    typedef asl::raster<asl::BGR>  BGRRaster;
    typedef asl::raster<asl::RGB>  RGBRaster;
    typedef asl::raster<asl::GRAY> GRAYRaster;

    /**
    * @ingroup Y60video
    * video processing interface class
    * implement onFrame for processing a frame
    * implement configure to handle xml-node parameters
    * parameters should contain necessary image ids
    * ouput is handled by result()
    */
    class Y60_VIDEOPROCESSING_DECL Algorithm {
    public:
        Algorithm(const std::string & theName) {
            _myName = theName;
        }
        virtual ~Algorithm() {};
        virtual void configure(const dom::Node & theNode) = 0;

        virtual const dom::Node & result() const {
            AC_WARNING << "Algorithm::result not implemented.";
            static dom::Node dummy;
            return dummy;
        }
        virtual void onFrame(double t) = 0;

        virtual void setScene(const y60::ScenePtr & theScene) {
            _myScene = theScene;
        }

        virtual std::string getAlgorithmName() const {
            return _myName;
        }

        inline
            void rgb_to_hsl(unsigned char theR, unsigned char theG, unsigned char theB,
            asl::Vector3f & theResult) {
                unsigned char myMax = asl::maximum(theR, asl::maximum(theG, theB));
                unsigned char myMin = asl::minimum(theR, asl::minimum(theG, theB));
                float myDelta = float(myMax - myMin);

                theResult[0] = 0;    // should be undefined (for grey colors)
                theResult[1] = myMax ? (255 * myDelta / myMax) : 0;
                theResult[2] = (myMin + myMax) / 2.0f;

                if (theResult[1]) {
                    if (theR == myMax) {
                        theResult[0] = (theG - theB) / myDelta;
                    } else if (theG == myMax) {
                        theResult[0] = 2 + (theB - theR) / myDelta;
                    } else if (theB == myMax) {
                        theResult[0] = 4 + (theR - theG) / myDelta;
                    }
                    theResult[0] *= 60;
                    if (theResult[0] < 0){
                        theResult[0] += 360;
                    }
                }
        }

        inline
            void rgb_to_intensity(const unsigned char theR, const unsigned char theG,
            const unsigned char theB, unsigned int & theResult) {
                theResult = static_cast<unsigned int>(0.2989*theR + 0.5870*theG + 0.1140*theB);
        }

        inline
            void rgb_to_intensity(const unsigned char theR, const unsigned char theG,
            const unsigned char theB, unsigned char & theResult) {
                theResult = static_cast<unsigned char>(0.2989*theR + 0.5870*theG + 0.1140*theB);
        }

    protected:
        y60::ScenePtr _myScene;
        std::string _myName;
    };

    typedef asl::Ptr<Algorithm> AlgorithmPtr;

    /**********************************************************/
    //demo algorithm

    class TestAlgorithm : public Algorithm {
    public:
        TestAlgorithm(const std::string & theName) : Algorithm( theName )  { }

        void configure(const dom::Node & theNode) {

        }
        static std::string getName() { return "test"; }

        void onFrame(double t) {
            int x = 0, y = 0;
            const BGRRaster * myFrame = dom::dynamic_cast_Value<BGRRaster>(_mySourceImage.get());
            std::cout << "raster size " << myFrame->xsize() << "x" << myFrame->ysize()
                << " value " << x << "," << y << " = "
                << int( getRedValue((*myFrame)(x,y)) ) << ","
                << int( getGreenValue((*myFrame)(x,y)) )<< ","
                << int( getBlueValue((*myFrame)(x,y)) )
                << std::endl;
        }
    private:
        dom::ValuePtr _mySourceImage;
    };
}

#endif

