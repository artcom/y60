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
//   $RCSfile: CharacterMover.cpp,v $
//   $Author: pavel $
//   $Revision: 1.14 $
//   $Date: 2005/04/24 00:41:19 $
//
//
//=============================================================================

#include "CharacterMover.h"
#include <asl/Time.h>
#include <cmath>

#define DB(x) // x

using namespace y60;
using namespace asl;
using namespace std;

namespace jslib {

    float getRandomNumber() {
        static bool isSeeded = false;
        if (!isSeeded) {
            asl::Time myTime;
            srand(myTime.usecs());
            isSeeded = true;
        }
        return (rand() / float(RAND_MAX));
    }

    CharacterMover::CharacterMover(
                const asl::Vector3f & theStartPosition,
                const asl::Vector3f & theFinalPosition,
                const asl::Vector3f & theFinalPositionOffset
    ) :
        _myState(SLEEPING),
        _myStartPosition(theStartPosition),
        _myFinalPosition(theFinalPosition),
        _myFinalPositionOffset(theFinalPositionOffset),
        _myStateDuration(0),
        _myGoToRelevance(1.0f),
        _myMaxForce(0.0f),
        _myMaxSpeed(0.0f),
        _myMass(0.0f),
        _myCellSize(0.f, 0.f, 0.f),
        _myPositionIsDirty(false),
        _myColorIsDirty(false)
    {
        setState(SLEEPING);
    }

    bool
    CharacterMover::isInside(asl::Vector2f theCenter, float theRadius) {
        return (magnitude(difference(theCenter,
            Vector2f(_myCurrentPosition[0] + (_myCellSize[0] / 2),
                     _myCurrentPosition[1] + (_myCellSize[1] / 2)))) <= theRadius);

        //return (fabs(theCenter[0] - _myCurrentPosition[0]) < theRadius &&
        //        fabs(theCenter[1] - _myCurrentPosition[1]) < theRadius);
    }

    void
    CharacterMover::setState(State theNewState) {
        DB(AC_TRACE << "setState" << int(theNewState) << endl;)
        if (_myState == theNewState) {
            return;
        }
        switch (theNewState) {
            case SLEEPING:
                _myGoToRelevance      = 0.2f;
                _myMaxForce           = 0.3f;
                _myMaxSpeed           = 30;
                _myMass               = 1;
                setAlpha(0.0f);
                break;
            case EXPLODING:
            {
                // set point to go to
                float myRandomValue = getRandomNumber() * 360;
                float myRadius = 100 + getRandomNumber() * 400;

                _myAim = Vector3f(sin(myRandomValue) * myRadius + _myStartPosition[0],
                    cos(myRandomValue) * myRadius + _myStartPosition[1], 0);

                _myVelocity           = Vector3f(0, 3, 0);
                _myGoToRelevance      = 1;
                _myMaxForce           = 0.6f + getRandomNumber() * 0.3f;
                _myMaxSpeed           = 10 + getRandomNumber() * 150;
                _myMass               = 0.1f + getRandomNumber() * 0.1f;

                setPosition(_myStartPosition);
                break;
            }

            case FINDING:
                _myAim                = _myFinalPosition + _myFinalPositionOffset;
                _myGoToRelevance      = 0.8f;
                _myMaxForce           = 1;
                _myMaxSpeed           = 300;
                _myMass               = 0.1f;
                break;

            case REMAINING:
                _myGoToRelevance      = 0;
                _myMaxForce           = 0.1f;
                _myMaxSpeed           = 30;
                _myMass               = 1;
                setAlpha(1.0f);
                break;

            case LEAVING:
            {
                float myRandomValue = getRandomNumber() * 360;
                float myRadius = 10 + getRandomNumber() * 150;
                _myAim = Vector3f(sin(myRandomValue) * myRadius + _myCurrentPosition[0],
                    cos(myRandomValue) * myRadius + _myCurrentPosition[1], 0);

                _myVelocity = Vector3f(30, 0, 0);
                _myGoToRelevance      = 0.4f;
                _myMaxForce           = 0.4f;
                _myMaxSpeed           = 60;
                _myMass               = 2;
                setAlpha(1.0f);
                break;
            }
        }

        _myStateDuration = 0;
        _myState = theNewState;
    }

    CharacterMover::State
    CharacterMover::onFrame(double theDeltaTime, Vector4f * theColors,
        Vector3f * thePositions, Box3f & theBoundingBox)
    {
        _myStateDuration += theDeltaTime;

        switch (_myState) {
            case EXPLODING:
                if (_myStateDuration > 0.3 && _myMaxSpeed > 6) {
                    _myMaxSpeed -= 5.4f;
                }
                if (_myStateDuration > 0.6 && _myMaxForce > 0.2f) {
                    _myMaxForce -= 0.1f;
                }
                if (_myStateDuration >= 0.8) {
                    setState(FINDING);
                }
                setAlpha(1.0f);
                break;
            case FINDING:
            {
                if (_myStateDuration > 0.3) {
                    _myMaxForce = 0.5f;
                }
                if (_myMass > 0.2f) {
                    _myMass -= 0.07f;
                }

                float myDistance = magnitude(difference(_myAim, _myCurrentPosition));
                if (myDistance < 10) {
                    _myMaxSpeed = myDistance * 12;
                    _myMaxForce = myDistance / 14;
                }
                if (myDistance < 1) {
                    setPosition(_myFinalPosition + _myFinalPositionOffset);
                    setState(REMAINING);
                }
                setAlpha(1.0f);
                break;
            }
            case REMAINING:
                break;

            case LEAVING:
            {
                if (_myCurrentAlpha > 0) {
                    setAlpha(_myCurrentAlpha - 0.05f);
                }
                if (_myCurrentAlpha <= 0) {
                    setState(SLEEPING);
                }
                break;
            }
            case SLEEPING:
                break;
        }

        if (_myState != REMAINING && _myState != SLEEPING) {
            Vector3f myGotoDirection = normalized(difference(_myAim, _myCurrentPosition));
            Vector3f mySteeringForce = product(myGotoDirection, _myGoToRelevance);

            if (magnitude(mySteeringForce) > _myMaxForce) {
                mySteeringForce = product(normalized(mySteeringForce), _myMaxForce);
            }

            Vector3f myAcceleration = product(mySteeringForce, float(theDeltaTime * 900 / _myMass));
            _myVelocity.add(myAcceleration);
            if (magnitude(_myVelocity) > _myMaxSpeed) {
                _myVelocity = product(normalized(_myVelocity), _myMaxSpeed);
            }
            setPosition(sum(_myCurrentPosition, product(_myVelocity, float(theDeltaTime))));
        }

        updateVertexData(thePositions, theColors, theBoundingBox);
        return _myState;
    }

    void
    CharacterMover::updateVertexData(Vector3f * thePositions, Vector4f * theColors,
        Box3f & theBoundingBox)
    {
        if (_myCellSize == Vector3f(0.f, 0.f,0.f)) {
            _myCellSize = thePositions[2] - thePositions[0];
            _myCellSize[3] = 0.001f;    // ensures that the bounding box is non-empty
        }

        if (_myPositionIsDirty) {
            // manipulate AGP memory
            thePositions[0] = _myCurrentPosition;
            thePositions[1] = _myCurrentPosition + Vector3f(_myCellSize[0], 0.0f, 0.0f);
            thePositions[2] = _myCurrentPosition + Vector3f(_myCellSize[0], _myCellSize[1], 0.0f);
            thePositions[3] = _myCurrentPosition + Vector3f(0.0f, _myCellSize[1], 0.0f);
            _myPositionIsDirty = false;
        }

        if (_myColorIsDirty) {
            theColors[1] = theColors[2] = theColors[3] =
            theColors[0] = Vector4f(1.0f, 1.0f, 1.0f, _myCurrentAlpha);
            _myColorIsDirty = false;
        }

        // TODO: Find optimization
        theBoundingBox.extendBy(_myCurrentPosition);
        theBoundingBox.extendBy(_myCurrentPosition + _myCellSize);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //
    // String Mover (To move lots of characters around)
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////

    StringMover::StringMover(const asl::Vector3f & theStartPosition,
            const asl::Vector3f & theTargetPositionOffset,
            dom::NodePtr theShapeNode,
            dom::NodePtr theBodyNode) :
        _myStartPosition(theStartPosition),
        _myTargetPositionOffset(theTargetPositionOffset),
        _myLastIdleTime(0)
    {
        _myShape = theShapeNode->getFacade<y60::Shape>();
        _myBody  = theBodyNode->getFacade<y60::Body>();
    }

    void
    StringMover::resetPositions(const asl::Vector3f & theStartPosition,
        const asl::Vector3f & theTargetPositionOffset)
    {
        _myStartPosition = theStartPosition;
        _myTargetPositionOffset = theTargetPositionOffset;
        unsigned mySize = _myCharacters.size();
        for (unsigned i = 0; i < mySize; ++i) {
            _myCharacters[i].setStartPosition(theStartPosition);
            _myCharacters[i].setTargetPositionOffset(theTargetPositionOffset);
        }
    }

    void
    StringMover::setState(int theNewState) {
        _myState = CharacterMover::State(theNewState);
        _myBody->set<VisibleTag>(_myState != CharacterMover::SLEEPING);

        unsigned mySize = _myCharacters.size();
        for (unsigned i = 0; i < mySize; ++i) {
            _myCharacters[i].setState(CharacterMover::State(theNewState));
        }
    }

    void
    StringMover::onMouseButton(Vector2f theMousePosition) {
        unsigned mySize = _myCharacters.size();
        for (unsigned i = 0; i < mySize; ++i) {
            if (_myCharacters[i].isInside(theMousePosition, 10)) {
                _myCharacters[i].setStartPosition(_myCharacters[i].getPosition());
                _myCharacters[i].setState(CharacterMover::EXPLODING);
                _myState = CharacterMover::EXPLODING;
            }
        }
    }

    void
    StringMover::onFrame(double theTime) {
        double myDeltaTime = theTime - _myLastIdleTime;
        _myLastIdleTime = theTime;

        if (_myState != CharacterMover::REMAINING && _myState != CharacterMover::SLEEPING) {
            PrimitiveVector & myElements = _myShape->getPrimitives();

            Box3f myBoundingBox;
            myBoundingBox.makeEmpty();

            _myState = CharacterMover::SLEEPING;
            unsigned myOffset = 0;

            unsigned myElementCount = myElements.size();
            for (unsigned i = 0; i < myElementCount; ++i) {
#ifdef OLD
                VertexData4f::VertexDataVector * myColors = myElements[i].getColors();
                VertexData3f::VertexDataVector * myPositions = myElements[i].getPositions();

#else
                
                Ptr<VertexDataAccessor<Vector3f> > myPositionsAccessor = myElements[i]->getLockingPositionsAccessor();
                VertexData3f * myPositions = &myPositionsAccessor->get();

                Ptr<VertexDataAccessor<Vector4f> > myColorsAccessor = myElements[i]->getLockingColorsAccessor();
                VertexData4f * myColors = &myColorsAccessor->get();
#endif                
                unsigned myPrimitveCount = myPositions->size() / 4;
                for (unsigned j = 0; j < myPrimitveCount; ++j) {
                    unsigned myIndex = j + myOffset;

                    CharacterMover::State myState = _myCharacters[myIndex].onFrame(
                            myDeltaTime, &(*myColors)[j * 4],
                            &(*myPositions)[j * 4], myBoundingBox);

                    // Set string state to minimum character state
                    if (myState < _myState) {
                        _myState = myState;
                    }
                }
                myOffset += myPrimitveCount;
            }

            _myShape->set<BoundingBoxTag>(myBoundingBox);
            _myBody->set<VisibleTag>(_myState != CharacterMover::SLEEPING);
            _myShape->setLastRenderVersion(_myShape->getNode().nodeVersion()+1);
        }
    }

    void
    StringMover::addCharacter(const asl::Vector3f & theFinalPosition) {
        _myCharacters.push_back(CharacterMover(_myStartPosition, theFinalPosition, _myTargetPositionOffset));
    }
}
