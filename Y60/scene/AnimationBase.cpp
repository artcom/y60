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
//   $RCSfile: AnimationBase.cpp,v $
//   $Author: christian $
//   $Revision: 1.10 $
//   $Date: 2005/04/24 17:56:48 $
//
//  Description: Animation utils.
//
//=============================================================================

#include "AnimationBase.h"

#include <y60/NodeNames.h>

#include <dom/Nodes.h>
#include <asl/Logger.h> 

#include <iostream>

using namespace std;

#define DB(x)  //   x
#define DB_2(x) //  x

namespace y60 {

    AnimationBase::AnimationBase(dom::NodePtr theNode, bool theAngleAnimation)
        :  _myNode(theNode),
           _myBegin(0),
           _myDuration(0),
           _myIn(0),
           _myOut(0),
           _myPause(0),
           _myCount(1),
           _myAngleAnimation(theAngleAnimation),
           _myPlayType(FORWARD),
           _myCurrentDirection(FORWARD),
           _myPreviousAnimTime(-1),
           _myEffectiveRuntime(0)
    {
        update();
    }

    void
    AnimationBase::update() {
        dom::NodePtr myAttribute = _myNode->getAttribute(ANIM_BEGIN_ATTRIB);
        if (myAttribute) {
            _myBegin = myAttribute->dom::Node::nodeValueRef<float>();
        }

        myAttribute = _myNode->getAttribute(ANIM_DURATION_ATTRIB);
        if (myAttribute) {
            _myDuration = myAttribute->dom::Node::nodeValueRef<float>();
        }

        myAttribute = _myNode->getAttribute(ANIM_CLIPIN_ATTRIB);
        if (myAttribute) {
            _myIn = myAttribute->dom::Node::nodeValueRef<float>();
        }

        myAttribute = _myNode->getAttribute(ANIM_CLIPOUT_ATTRIB);
        if (myAttribute) {
            _myOut = myAttribute->dom::Node::nodeValueRef<float>();
        }

        myAttribute = _myNode->getAttribute(ANIM_PAUSE_ATTRIB);
        if (myAttribute) {
            _myPause = myAttribute->dom::Node::nodeValueRef<float>();
        }

        myAttribute = _myNode->getAttribute(ANIM_COUNT_ATTRIB);
        if (myAttribute) {
            _myCount = myAttribute->dom::Node::nodeValueRef<unsigned>();
        }

        myAttribute = _myNode->getAttribute(ANIM_DIRECTION_ATTRIB);
        if (myAttribute) {
            _myPlayType = (AnimationDirection)(asl::getEnumFromString(
                myAttribute->nodeValue(), AnimationDirectionString));
        }

        _myCurrentDirection = _myPlayType;

        // ???
        //if (_myPlayType == REVERSE) {
        //    _myCurrentDirection = REVERSE;
        //}
    }

    bool
    AnimationBase::isEnabled() const {
        return _myNode->dom::Node::getAttributeValue<bool>(ANIM_ENABLED_ATTRIB);
    }

    double
    AnimationBase::calcEndTime() {
        return  getRealDuration() * _myCount;
    }

    unsigned int
    AnimationBase::getLoops() {
        double myDuration = getRealDuration();
        if (myDuration) {
            return (unsigned int)(_myEffectiveRuntime / myDuration);
        } else {
            return 0;
        }
    }

    double
    AnimationBase::getRealDuration() {
       if (_myOut != 0) {
            return _myIn + (_myOut - _myIn);
        } else {
            return (_myDuration + _myPause);
        }
    }

    unsigned int
    AnimationBase::calcIndicies(int & theLowerIndex, int & theUpperIndex, double & theAlpha,
                  const double & theTime, const int & theValueSize)
    {
        // Animation has not started, yet.
        if (theTime < _myBegin + _myIn) {
            DB(AC_DEBUG << "theTime : " << theTime << " _myBegin" << _myBegin << " _myIn : " << _myIn << endl);
            DB(AC_DEBUG << "--------------------- nothing to do " << endl);
            return 0;
        }
        _myEffectiveRuntime = theTime - _myBegin;
        double myAnimationTime =_myEffectiveRuntime;
        double myAnimationEndTime = calcEndTime();


        DB(AC_DEBUG <<"myAnimationTime : " << myAnimationTime << " will end at " << myAnimationEndTime << endl);
        DB(AC_DEBUG <<"count : " << _myCount << endl);
        DB(AC_DEBUG <<"Timing: begin: " << _myBegin << " in : " << _myIn << " out : " << _myOut << endl);
        // Count = 0 means infinite loops -> no end
        if (_myCount) {
            if (myAnimationTime > myAnimationEndTime) {
                if (_myOut != 0) {
                    DB(AC_DEBUG <<"-> the Out is set and the animation is done" << endl);
                    DB(AC_DEBUG << "--------------------- nothing to do " << endl);
                    return 0;
                } else {
                    DB(AC_DEBUG <<"-> the animation is done" << endl);
                    DB(AC_DEBUG << "--------------------- setToEndValue " << endl);
                    return 1;
                }
            }
        }

        // Calculate time offset from start of animation cycle
        if (_myOut == 0 && _myIn ==0) {
            myAnimationTime = fmod(myAnimationTime, double((_myDuration + _myPause)));
        } else {
            if (myAnimationTime > _myOut) {
                double myDiffToIn = fmod((myAnimationTime - _myIn) , double(_myOut - _myIn));
                DB_2(AC_TRACE <<"##### Looping "<< endl);
                myAnimationTime = _myIn + myDiffToIn;
            }
        }
        DB(AC_DEBUG <<"                                      AnimationTime gymnastic: " << myAnimationTime << endl);

        // Detect end of cycle
        if (myAnimationTime < _myPreviousAnimTime) {
            // Toggle pong direction
            if (_myPlayType == PONG) {
                if (_myCurrentDirection == FORWARD) {
                    _myCurrentDirection = REVERSE;
                } else {
                    _myCurrentDirection = FORWARD;
                }
            }
        }
        _myPreviousAnimTime = myAnimationTime;


        if (myAnimationTime > _myDuration) {
            AC_DEBUG << " -> animation has ended" << endl;
            DB(AC_DEBUG << "--------------------- setToEndValue " << endl);
            return 1;
        }

        // Flip direction for reverse mode
        if (_myCurrentDirection == REVERSE) {
            DB(AC_DEBUG <<" ###  reverse the animation" << endl);
            myAnimationTime = _myDuration - myAnimationTime;
        }


        // calc the real indicies
        double myValuePosition = myAnimationTime / _myDuration * (theValueSize - 1);
        theLowerIndex = int(myValuePosition);
        theUpperIndex = theLowerIndex + 1;
        if (theUpperIndex >= theValueSize) {
            theUpperIndex = 0;
        }
        theAlpha = myValuePosition - theLowerIndex;
        DB(AC_TRACE <<"frames : " << theValueSize << endl);
        DB(AC_TRACE <<" Play Frames : " << theLowerIndex << " & "  << theUpperIndex << " , alpha: "<< theAlpha << endl);
        DB(AC_DEBUG << "--------------------- animate " << endl);
        return 2;
    }

    bool
    AnimationBase::isAngleAnimation() {
        return _myAngleAnimation;
    }

    AnimationDirection
    AnimationBase::getCurrentDirection() {
        return _myCurrentDirection;
    }

    void
    AnimationBase::play(double theTime) {
        _myNode->getAttribute(ANIM_ENABLED_ATTRIB)->nodeValueAssign<bool>(true);
        _myNode->getAttribute(ANIM_BEGIN_ATTRIB)->nodeValueAssign<float>(float(theTime));
        update();
        DB(AC_DEBUG <<" Animation: " << _myNode->getAttributeString(NAME_ATTRIB) << " started" << endl);
    }

    void
    AnimationBase::setLoops(unsigned int theLoops) {
        _myNode->getAttribute(ANIM_COUNT_ATTRIB)->nodeValueAssign<unsigned int>(theLoops);
        _myCount = theLoops;
        //update();
    }
    void
    AnimationBase::setForwardDirection(bool theFlag) {
        if (theFlag) {
            _myNode->getAttribute(ANIM_DIRECTION_ATTRIB)->nodeValue("forward");
            _myPlayType = FORWARD;
        } else {
            _myNode->getAttribute(ANIM_DIRECTION_ATTRIB)->nodeValue("reverse");
            _myPlayType = REVERSE;
        }
        _myCurrentDirection = _myPlayType;
        //update();
    }

}
