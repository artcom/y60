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

#ifndef _AC_BLOBS_H_
#define _AC_BLOBS_H_

#include "y60_videoprocessing_settings.h"

#include "Algorithm.h"
#include <y60/scene/Overlay.h>
#include <y60/image/ConnectedComponent.h>

namespace y60 {

struct Cursor {
    Cursor() :
            position(0.0, 0.0), 
            motion(0.0, 0.0),
            correlatedPosition(-1),
            creationTime(0.0)
    {
        roi.makeEmpty();
        previousRoi.makeEmpty();
    }

    Cursor(const asl::Vector2f theCenter, const asl::Box2f & theBox, double theTime) :
            position( theCenter ), 
            roi( theBox),
            motion(0.0,0.0),
            correlatedPosition(-1),
            creationTime(theTime)
    {
        previousRoi.makeEmpty();
    }

    asl::Vector2f position;
    asl::Box2f    roi;
    asl::Box2f    previousRoi;

    asl::Vector2f motion;
    int correlatedPosition;
    double creationTime;
};

typedef std::map<int, Cursor> CursorMap;


	class Y60_VIDEOPROCESSING_DECL Blobs : public Algorithm {
		public:
            Blobs(const std::string & theName);

			static std::string getName() { return "blobs"; }
		    void onFrame(double t);
            
            void configure(const dom::Node & theNode);
	        const dom::Node & result() const { 
		        return _myResultNode;
	        }


		private:
            void correlatePositions(BlobListPtr & theBlobs, double theDeltaT);
            asl::Matrix4f getTransformationMatrix();

            float lookup(float theValue);
            float revlookup(float theValue);
            dom::Element  _myResultNode;
            y60::ImagePtr _mySourceImage;
            y60::ImagePtr _myTargetImage;
            
            int _myThreshold;
            
            CursorMap   _myCursors;
            dom::NodePtr _myOverlay;
            int _myIDCounter;
            float _myDistanceThreshold;
            int _myCarCounter;
    };
}

#endif

