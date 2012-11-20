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
*/
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

#include "y60_scene_settings.h"

#include "AnimationBase.h"
#include "AnimationCharacter.h"

#include <asl/base/Exception.h>
#include <asl/dom/Nodes.h>

#include <string>
#include <vector>

namespace y60 {

    DEFINE_EXCEPTION(AnimationManagerException, asl::Exception);

    class Y60_SCENE_DECL AnimationManager {
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

            std::string splitNameAndComponent( const std::string & theName,
                    AnimationBase::AttributeComponent & theComponent );
            AnimationClipPtr         _myGlobalAnimations;
            AnimationCharacterMap    _myCharacter;

            dom::NodePtr findAnimatedValue(dom::NodePtr theNode,
                    const std::string & theAttributeRef,
                    AnimationBase::AttributeComponent & myAttributeComponent);
    };
}

#endif
