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

#include "y60_scene_settings.h"

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
            void reload(dom::NodePtr theNode, AnimationManager & theAnimationManager, dom::NodePtr theWorld);

            unsigned int getLoops();

        private:
            void load(AnimationManager & theAnimationManager, dom::NodePtr theWorld);

            AnimationVector  _myAnimations;
            std::string      _myName;
            dom::NodePtr     _myNode;
            double           _myStartTime;
            bool             _isActive;
            asl::Unsigned64  _myDomVersion;

    };
    typedef asl::Ptr<AnimationClip> AnimationClipPtr;
    typedef std::vector<AnimationClipPtr> AnimationClips;
    typedef std::map<std::string, AnimationClipPtr> AnimationClipMap;

}

#endif

