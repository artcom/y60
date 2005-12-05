//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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

#include <asl/Exception.h>
#include <asl/Ptr.h>
#include <dom/Nodes.h>
#include <y60/NodeValueNames.h>

#include <string>
#include <math.h>
#include <vector>

namespace y60 {

    DEFINE_EXCEPTION(AnimationException, asl::Exception);


    class AnimationBase {
        public:
            enum VectorComponent {
                SCALAR = -1,
                X,
                Y,
                Z,
                W
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

