//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: AnimationCharacter.h,v $
//   $Author: janbo $
//   $Revision: 1.6 $
//   $Date: 2004/10/18 16:22:59 $
//
//  Description: A simple animation class.
//
//=============================================================================

#ifndef _ac_scene_AnimationCharacter_h_
#define _ac_scene_AnimationCharacter_h_

#include "AnimationClip.h"
#include <dom/Nodes.h>
#include <asl/Exception.h>

#include <string>
#include <vector>
#include <map>

namespace y60 {
    DEFINE_EXCEPTION(AnimationCharacterException, asl::Exception);

    class AnimationManager;
    class AnimationCharacter{
        public:
            AnimationCharacter(dom::NodePtr theNode, AnimationManager & theAnimationManager, dom::NodePtr theWorld);
            virtual ~AnimationCharacter();
            void run(double theTime);
            void playClip(double theTime, const std::string & theClipName);
            void setClipLoops(const std::string & theClipName, unsigned int theLoops);
            void setClipForwardDirection(const std::string & theClipName, bool theFlag);
            unsigned int getLoops(const std::string & theClipName);
            bool isClipActive(const std::string & theClipName);
            bool isActive();
            void stop();
        private:
            AnimationClipPtr getClip(const std::string & theClipName, const std::string & theContext);
            void load(AnimationManager & theAnimationManager, dom::NodePtr theWorld);

            std::string      _myName;
            dom::NodePtr     _myNode;
            AnimationClipPtr _myActiveClip;
            AnimationClipMap _myClips;
    };
    typedef asl::Ptr<AnimationCharacter> AnimationCharacterPtr;
    typedef std::map<std::string, AnimationCharacterPtr> AnimationCharacterMap;

}

#endif

