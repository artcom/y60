//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _plugin_CharacterMover_h_
#define _plugin_CharacterMover_h_

#include <y60/VertexData.h>
#include <y60/Shape.h>
#include <y60/Body.h>
#include <dom/Nodes.h>
#include <asl/Vector234.h>
#include <vector>

namespace jslib {

class CharacterMover {
    public:
        enum State {
            EXPLODING,
            FINDING,
            REMAINING,
            LEAVING,
            SLEEPING
        };

        CharacterMover(const asl::Vector3f & theStartPosition,
                       const asl::Vector3f & theFinalPosition,
                       const asl::Vector3f & theFinalPositionOffset = asl::Vector3f(0.f, 0.f, 0.f));

        State onFrame(double theDeltaTime, asl::Vector4f * theColors,
            asl::Vector3f * thePositions, asl::Box3f & theBoundingBox);

        bool isInside(asl::Vector2f theCenter, float theRadius);

        void setState(State theNewState);
        State getState() const {
            return _myState;
        }

        const asl::Vector3f & getPosition() const {
            return _myCurrentPosition;
        }

        void setStartPosition(const asl::Vector3f & theStartPosition) {
            _myStartPosition       = theStartPosition;
        }

        void setTargetPositionOffset(const asl::Vector3f & theFinalPositionOffset) {
            _myFinalPositionOffset = theFinalPositionOffset;
        }

    private:
        void setAlpha(float theNewAlpha) {
            _myCurrentAlpha = theNewAlpha;
            _myColorIsDirty = true;
        }

        void setPosition(const asl::Vector3f & theNewPosition) {
            _myCurrentPosition = theNewPosition;
            _myPositionIsDirty = true;
        }
        void updateVertexData(asl::Vector3f * thePositions, asl::Vector4f * theColors,
            asl::Box3f & theBoundingBox);

        asl::Vector3f   _myCellSize;

        asl::Vector3f   _myCurrentPosition;
        float           _myCurrentAlpha;
        bool            _myPositionIsDirty;
        bool            _myColorIsDirty;

        asl::Vector3f   _myStartPosition;
        asl::Vector3f   _myFinalPosition;
        asl::Vector3f   _myFinalPositionOffset;

        double _myStateDuration;
        State _myState;

        // Kinematic
        asl::Vector3f _myAim;
        asl::Vector3f _myVelocity;

        float _myMaxForce;
        float _myMaxSpeed;
        float _myMass;

        // Relevances
        float _myGoToRelevance;
};

class StringMover {
    public:
        StringMover(const asl::Vector3f & theStartPosition,
            const asl::Vector3f & theTargetPositionOffset,
            dom::NodePtr theShapeNode,
            dom::NodePtr theBodyNode
        );

        void setState(int theNewState);
        int getState() const {
            return int(_myState);
        }
        void onFrame(double theTime);
        void addCharacter(const asl::Vector3f & theFinalPosition);
        void onMouseButton(asl::Vector2f theMousePosition);


        void resetPositions(const asl::Vector3f & theStartPosition,
            const asl::Vector3f & theTargetPositionOffset);

    private:
        asl::Vector3f               _myStartPosition;
        asl::Vector3f               _myTargetPositionOffset;
        y60::ShapePtr               _myShape;
        y60::BodyPtr                _myBody;
        std::vector<CharacterMover> _myCharacters;
        CharacterMover::State       _myState;
        double _myLastIdleTime;
};

}

#endif

