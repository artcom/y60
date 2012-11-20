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

#ifndef _plugin_CharacterMover_h_
#define _plugin_CharacterMover_h_

#include "y60_jslib_settings.h"

#include <y60/scene/VertexData.h>
#include <y60/scene/Shape.h>
#include <y60/scene/Body.h>
#include <asl/dom/Nodes.h>
#include <asl/math/Vector234.h>
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

