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
//   $RCSfile: AnimationManager.h,v $
//   $Author: janbo $
//   $Revision: 1.11 $
//   $Date: 2004/10/18 16:22:59 $
//
//  Description: This class holds all animations.
//
//=============================================================================

#ifndef _ac_scene_AnimationManager_h_
#define _ac_scene_AnimationManager_h_

#include "AnimationBase.h"
#include "AnimationCharacter.h"

#include <asl/Exception.h>
#include <dom/Nodes.h>

#include <string>
#include <vector>

namespace y60 {

    DEFINE_EXCEPTION(AnimationManagerException, asl::Exception);

    class AnimationManager {
        public:
            AnimationManager();
            virtual ~AnimationManager();

            void loadGlobals(const dom::NodePtr theNode, dom::NodePtr theWorld);
            void loadCharacters(const dom::NodePtr theNode, dom::NodePtr theWorld);
            void run(double theTime);
            void playClip(double theTime, const std::string & theCharacterName, const std::string & theClipName);
            void update();

            bool isCharacterActive(const std::string & theCharacterName);
            void stop(const std::string & theCharacterName);
            bool isClipActive(const std::string & theCharacterName, const std::string & theClipName);
            void setClipLoops(const std::string & theCharacterName, const std::string & theClipName, unsigned theLoops);
            void setClipForwardDirection(const std::string & theCharacterName, const std::string & theClipName, bool theFlag);            
            unsigned int getLoops(const std::string & theCharacterName,const std::string & theClipName);
            AnimationPtr createAnimation(const dom::NodePtr theNode, dom::NodePtr theWorld);
        private:
            AnimationCharacterPtr getCharacter(const std::string & theCharacterName, const std::string & theContext);

            AnimationClipPtr         _myGlobalAnimations;
            AnimationCharacterMap    _myCharacter;

            dom::NodePtr findAnimatedValue(dom::NodePtr theNode,
                    const std::string & theAttributeRef,
                    AnimationBase::VectorComponent & myVectorComponent);
    };
}

#endif
