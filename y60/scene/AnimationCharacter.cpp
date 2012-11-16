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
//   $RCSfile: AnimationCharacter.cpp,v $
//   $Author: pavel $
//   $Revision: 1.7 $
//   $Date: 2005/04/24 00:41:21 $
//
//  Description: Animation utils.
//
//=============================================================================

// own header
#include "AnimationCharacter.h"

#include "AnimationManager.h"

#include <y60/base/NodeNames.h>
#include <asl/base/string_functions.h>

#include <math.h>
#include <iostream>

using namespace std;
using namespace dom;

#define DB(x)  //   x

namespace y60 {

    AnimationCharacter::AnimationCharacter(NodePtr theNode, AnimationManager & theAnimationManager,
                                           dom::NodePtr theWorld) :
            _myNode(theNode)
    {
        _myActiveClip = AnimationClipPtr();
        load(theAnimationManager, theWorld);
    }

    AnimationCharacter::~AnimationCharacter() {
    }

    void
    AnimationCharacter::load(AnimationManager & theAnimationManager, dom::NodePtr theWorld) {
        dom::NodePtr myAttribute = _myNode->getAttribute(NAME_ATTRIB);
        if (myAttribute) {
            _myName = myAttribute->dom::Node::nodeValue();
        }
        DB(AC_DEBUG << "Character loaded: " << _myName << endl);

       unsigned myClipCount = _myNode->childNodesLength();
        for (unsigned i = 0; i < myClipCount; ++i) {
            const dom::NodePtr myChildNode = _myNode->childNode(i);
            if (myChildNode->nodeType() == dom::Node::ELEMENT_NODE &&
                myChildNode->nodeName() == CLIP_NODE_NAME)
            {
                string myClipName = myChildNode->dom::Node::getAttributeString(NAME_ATTRIB);
                if (_myClips.find(myClipName) == _myClips.end() ) {
                    _myClips[myClipName] = AnimationClipPtr(new AnimationClip(myChildNode,
                                                                              theAnimationManager, theWorld));
                } else {
                    throw AnimationCharacterException(string("Duplicate clip: ") + myClipName, PLUS_FILE_LINE);
                }
            }
        }
    }

    void
    AnimationCharacter::run(double theTime) {
        if (_myActiveClip) {
            _myActiveClip->run(theTime);
            if (!_myActiveClip->isActive()) {
                _myActiveClip = AnimationClipPtr();
            }
        }
    }

    void
    AnimationCharacter::playClip(double theTime, const string & theClipName) {
        _myActiveClip = getClip(theClipName, PLUS_FILE_LINE);
        _myActiveClip->play(theTime);
    }

    bool
    AnimationCharacter::isClipActive(const std::string & theClipName) {
        return getClip(theClipName, PLUS_FILE_LINE)->isActive();
    }

    bool
    AnimationCharacter::isActive() {
        if (_myActiveClip) {
            return _myActiveClip->isActive();
        }
        return false;
    }

    void
    AnimationCharacter::stop() {
        _myActiveClip = AnimationClipPtr();
    }

    void
    AnimationCharacter::setClipLoops(const std::string & theClipName, unsigned int theLoops) {
        getClip(theClipName, PLUS_FILE_LINE)->setLoops(theLoops);
    }

    void
    AnimationCharacter::setClipForwardDirection(const std::string & theClipName, bool theFlag) {
        getClip(theClipName, PLUS_FILE_LINE)->setForwardDirection(theFlag);
    }

    unsigned int
    AnimationCharacter::getLoops(const std::string & theClipName) {
        return getClip(theClipName, PLUS_FILE_LINE)->getLoops();
    }

    AnimationClipPtr
    AnimationCharacter::getClip(const std::string & theClipName, const std::string & theContext = PLUS_FILE_LINE) {
       if (_myClips.find(theClipName) != _myClips.end() ) {
            return _myClips[theClipName];
        } else {
            throw AnimationCharacterException(string("Unknown clip: ") + theClipName, theContext);
        }
    }

}
