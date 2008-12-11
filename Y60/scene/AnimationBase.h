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
//
//   $RCSfile: AnimationBase.h,v $
//   $Author: christian $
//   $Revision: 1.9 $
//   $Date: 2005/04/24 17:56:48 $
//
//  Description: A simple animation class.
//
//=============================================================================

#ifndef _ac_scene_AnimationBase_h_
#define _ac_scene_AnimationBase_h_

#include <asl/base/Exception.h>
#include <asl/base/Ptr.h>
#include <asl/dom/Nodes.h>
#include <y60/base/NodeValueNames.h>

#include <string>
#include <math.h>
#include <vector>

namespace y60 {

    DEFINE_EXCEPTION(AnimationException, asl::Exception);


    class AnimationBase {
        public:
            enum AttributeComponent {
                SCALAR = -1,
                X,
                Y,
                Z,
                W,
                HFOV
            };

            AnimationBase(dom::NodePtr theNode, bool theAngleAnimation);
            virtual ~AnimationBase() {}

            virtual bool run(double theTime) = 0;
            void update();
            void play(double theTime);
            bool isEnabled() const;
            void setLoops(unsigned int theLoops);
            void setForwardDirection(bool theFlag);

            unsigned int getLoops();


        protected:
            unsigned int calcIndicies(int & theLowerIndex, int & theUpperIndex, double & theAlpha,
                  const double & theTime, const int & theValueSize);

            bool isAngleAnimation();
            AnimationDirection getCurrentDirection();

        private:
            double getRealDuration();
            double calcEndTime();

            dom::NodePtr     _myNode;
            float            _myBegin;
            float            _myDuration;
            float            _myIn;    // relative timing in seconds, animation will start begin + in
            float            _myOut;   // relative timing in seconds, animation will end at begin + end
            float            _myPause;
            unsigned         _myCount;
            bool             _myAngleAnimation;
            AnimationDirection _myPlayType;
            AnimationDirection _myCurrentDirection;
            double           _myPreviousAnimTime;
            double           _myEffectiveRuntime;
    };

    typedef asl::Ptr<AnimationBase> AnimationPtr;
    typedef std::vector<AnimationPtr> AnimationVector;
}

#endif

