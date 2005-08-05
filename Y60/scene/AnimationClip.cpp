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
//   $RCSfile: AnimationClip.cpp,v $
//   $Author: pavel $
//   $Revision: 1.6 $
//   $Date: 2005/04/24 00:41:21 $
//
//  Description: Animation utils.
//
//=============================================================================

#include "AnimationClip.h"
#include "AnimationManager.h"

#include <y60/NodeNames.h>
#include <asl/Exception.h>
#include <asl/string_functions.h>

#include <iostream>

using namespace std;
using namespace dom;

#define DB(x) //    x

namespace y60 {

    AnimationClip::AnimationClip(NodePtr theNode, AnimationManager & theAnimationManager, dom::NodePtr theWorld) :
            _myNode(theNode), _isActive(false)
    {
        load(theAnimationManager, theWorld);
    }

    AnimationClip::~AnimationClip() {
    }

    void
    AnimationClip::load(AnimationManager & theAnimationManager, dom::NodePtr theWorld) {
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
