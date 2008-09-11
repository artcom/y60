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
//   $RCSfile: AnimationClip.h,v $
//   $Author: janbo $
//   $Revision: 1.5 $
//   $Date: 2004/10/18 16:22:59 $
//
//  Description: A simple animation class.
//
//=============================================================================

#ifndef _ac_scene_AnimationClip_h_
#define _ac_scene_AnimationClip_h_

#include "AnimationBase.h"

#include <asl/dom/Nodes.h>

#include <string>
#include <vector>
#include <map>

namespace y60 {
    class AnimationManager;
    class AnimationClip{
        public:
            AnimationClip(dom::NodePtr theNode, AnimationManager & theAnimationManager, dom::NodePtr theWorld);
            virtual ~AnimationClip();

            void run(double theTime);
            void play(double theTime);
            void update();
            bool isActive();
            void setActive();
            void setLoops(unsigned int theLoops);
            void setForwardDirection(bool theFlag);            
            
            unsigned int getLoops();

        private:
            void load(AnimationManager & theAnimationManager, dom::NodePtr theWorld);

            AnimationVector  _myAnimations;
            std::string      _myName;
            dom::NodePtr     _myNode;
            double           _myStartTime;
            bool             _isActive;

    };
    typedef asl::Ptr<AnimationClip> AnimationClipPtr;
    typedef std::vector<AnimationClipPtr> AnimationClips;
    typedef std::map<std::string, AnimationClipPtr> AnimationClipMap;

}

#endif

