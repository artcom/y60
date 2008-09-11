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
//   $RCSfile: AnimationCharacter.cpp,v $
//   $Author: pavel $
//   $Revision: 1.7 $
//   $Date: 2005/04/24 00:41:21 $
//
//  Description: Animation utils.
//
//=============================================================================

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
        _myActiveClip = AnimationClipPtr(0);
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
                _myActiveClip = AnimationClipPtr(0);
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
        _myActiveClip = AnimationClipPtr(0);
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
