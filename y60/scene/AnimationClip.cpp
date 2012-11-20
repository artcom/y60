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
//   $RCSfile: AnimationClip.cpp,v $
//   $Author: pavel $
//   $Revision: 1.6 $
//   $Date: 2005/04/24 00:41:21 $
//
//  Description: Animation utils.
//
//=============================================================================

// own header
#include "AnimationClip.h"

#include "AnimationManager.h"

#include <y60/base/NodeNames.h>
#include <asl/base/Exception.h>
#include <asl/base/string_functions.h>

#include <iostream>

using namespace std;
using namespace dom;

#define DB(x) //    x

namespace y60 {

    AnimationClip::AnimationClip(NodePtr theNode, AnimationManager & theAnimationManager, dom::NodePtr theWorld) :
            _myNode(theNode), _isActive(false), _myDomVersion(0)
    {
        load(theAnimationManager, theWorld);
    }

    AnimationClip::~AnimationClip() {
    }

    void
    AnimationClip::load(AnimationManager & theAnimationManager, dom::NodePtr theWorld) {
        if (_myDomVersion == _myNode->nodeVersion()) {
            return;
        }
        dom::NodePtr myAttribute = _myNode->getAttribute(NAME_ATTRIB);
        if (myAttribute) {
            _myName = myAttribute->dom::Node::nodeValue();
        }
        DB(AC_DEBUG << "Clip loaded: " << _myName << endl);
        unsigned myAnimationCount = _myNode->childNodesLength();
        for (unsigned i = 0; i < myAnimationCount; ++i) {
            const dom::NodePtr myChildNode = _myNode->childNode(i);
            if (myChildNode->nodeType() == dom::Node::ELEMENT_NODE &&
                myChildNode->nodeName() == ANIMATION_NODE_NAME)
            {
                string myAnimName = "";
                dom::NodePtr myAnimNameAttribute = myChildNode->getAttribute(NAME_ATTRIB);
                if (myAnimNameAttribute) {
                    myAnimName = myAnimNameAttribute->dom::Node::nodeValue();
                }
                DB(AC_DEBUG << "     Animation created: " << myAnimName << endl);
                _myAnimations.push_back(theAnimationManager.createAnimation(myChildNode, theWorld));
            }
        }
        _myDomVersion = _myNode->nodeVersion();
    }
    void
    AnimationClip::reload(NodePtr theNode, AnimationManager & theAnimationManager, dom::NodePtr theWorld) {
        if (!theNode->isSameNode(_myNode)) {
            _myAnimations .resize(0);
             _isActive=false;
             _myDomVersion=0;
             _myNode = theNode;
             load(theAnimationManager, theWorld);
             return;
        }
        if (_myDomVersion == _myNode->nodeVersion()) {
            return;
        }
        dom::NodePtr myAttribute = _myNode->getAttribute(NAME_ATTRIB);
        if (myAttribute) {
            _myName = myAttribute->dom::Node::nodeValue();
        }
        DB(AC_DEBUG << "Clip reloaded: " << _myName << endl);
        unsigned myAnimationCount = _myNode->childNodesLength();
        unsigned myAnimation = 0;
        for (unsigned i = 0; i < myAnimationCount; ++i) {
            const dom::NodePtr myChildNode = _myNode->childNode(i);
            if (myChildNode->nodeType() == dom::Node::ELEMENT_NODE &&
                myChildNode->nodeName() == ANIMATION_NODE_NAME)
            {
                string myAnimName = "";
                dom::NodePtr myAnimNameAttribute = myChildNode->getAttribute(NAME_ATTRIB);
                if (myAnimNameAttribute) {
                    myAnimName = myAnimNameAttribute->dom::Node::nodeValue();
                }
                DB(AC_DEBUG << "     Animation created: " << myAnimName << endl);
                if (myAnimation < _myAnimations.size()) {
                    if (_myAnimations[myAnimation]->getNode()->isSameNode(myChildNode)) {
                        _myAnimations[myAnimation]->update();
                        myAnimation++;
                    } else {
                        _myAnimations.erase(_myAnimations.begin()+myAnimation, _myAnimations.end());
                    }
                } else {
                    _myAnimations.push_back(theAnimationManager.createAnimation(myChildNode, theWorld));
                    myAnimation++;
                }
            }
        }
        _myDomVersion = _myNode->nodeVersion();
    }

    void
    AnimationClip::update() {
        for (unsigned i = 0; i < _myAnimations.size(); ++i) {
            _myAnimations[i]->update();
        }
    }

    void
    AnimationClip::run(double theTime) {
        if (_isActive) {
            _isActive = false;
            unsigned myAnimationsCount = _myAnimations.size();
            for (unsigned i = 0; i < myAnimationsCount; ++i) {
                if (_myAnimations[i]->isEnabled() ) {
                    _isActive |= _myAnimations[i]->run(theTime);
                }
            }
        }
    }

    void
    AnimationClip::play(double theTime) {
       _myStartTime = theTime;
       unsigned myAnimationsCount = _myAnimations.size();
       for (unsigned i = 0; i < myAnimationsCount; ++i) {
            _myAnimations[i]->play(theTime);
       }
       _isActive = true;
    }

    void
    AnimationClip::setLoops(unsigned int theLoops) {
       unsigned myAnimationsCount = _myAnimations.size();
       for (unsigned i = 0; i < myAnimationsCount; ++i) {
            _myAnimations[i]->setLoops(theLoops);
       }
    }

    void
    AnimationClip::setForwardDirection(bool theFlag) {
       unsigned myAnimationsCount = _myAnimations.size();
       for (unsigned i = 0; i < myAnimationsCount; ++i) {
            _myAnimations[i]->setForwardDirection(theFlag);
       }
    }

    unsigned int
    AnimationClip::getLoops() {
        if (_myAnimations.size()) {
            return _myAnimations[0]->getLoops();
        } else {
            return 0;
        }
    }


    bool
    AnimationClip::isActive() {
        return _isActive;
    }
    void
    AnimationClip::setActive() {
        _isActive = true;
    }
}
