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

#ifndef _AC_BLOBS_H_
#define _AC_BLOBS_H_

#include "Algorithm.h"
#include <y60/Overlay.h>
#include <y60/ConnectedComponent.h>

namespace y60 {

struct Cursor {
    Cursor() :
            position(0.0, 0.0), 
            motion(0.0,0.0),
            correlatedPosition(-1)
    {
        roi.makeEmpty();
        previousRoi.makeEmpty();
    }

    Cursor(const asl::Vector2f theCenter, const asl::Box2f & theBox) :
            position( theCenter ), 
            roi( theBox),
            motion(0.0,0.0),
            correlatedPosition(-1)
    {
        previousRoi.makeEmpty();
    }

    asl::Vector2f position;
    asl::Box2f    roi;
    asl::Box2f    previousRoi;

    asl::Vector2f motion;
    int correlatedPosition;


};

typedef std::map<int, Cursor> CursorMap;


	class Blobs : public Algorithm {
		public:
            Blobs(const std::string & theName);

			static std::string getName() { return "blobs"; }
		    void onFrame(double t);
            
            void configure(const dom::Node & theNode);
	        const dom::Node & result() const { 
		        return _myResultNode;
	        }


		private:
            void groupBlobs( BlobListPtr & theBlobs);
            void correlatePositions( BlobListPtr & theBlobs);
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

