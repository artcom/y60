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

#ifndef Y60_ASS_CURSOR_INCLUDED
#define Y60_ASS_CURSOR_INCLUDED

#include "y60_asscore_settings.h"

namespace y60 {

    struct MaximumResults {
        asl::Vector2f center;
        float max;
    };


    class Cursor {

    public:


        Cursor()
            : position(0.0, 0.0),
            major_direction(0.0, 0.0),
            minor_direction(0.0, 0.0),
            intensity(0.0),
            previousIntensity(0.0),
            firstDerivative(0.0),
            lastTouchTime(0.0),
            motion(0.0,0.0),
            correlatedPosition(-1)
        {
            roi.makeEmpty();
            previousRoi.makeEmpty();
        }

        Cursor(const asl::Box2f & theBox)
            : position(0.0, 0.0),
            major_direction(0.0, 0.0),
            minor_direction(0.0, 0.0),
            roi( theBox),
            intensity(0.0),
            previousIntensity(0.0),
            firstDerivative(0.0),
            lastTouchTime(0.0),
            motion(0.0,0.0),
            correlatedPosition(-1)
        {
            previousRoi.makeEmpty();
        }

        asl::Vector2f position;
        asl::Vector2f major_direction;
        asl::Vector2f minor_direction;
        asl::Box2f    roi;
        asl::Box2f    previousRoi;
        float         intensity;
        float         previousIntensity;
        float         firstDerivative;
        double        lastTouchTime;

        std::deque<float> intensityHistory;
        asl::Vector2f motion;
        int correlatedPosition;
    };

    class MomentCursor : public Cursor {
    public:
        MomentCursor( const asl::MomentResults & theMomentResult, const asl::Box2f & theBox )
            : Cursor( theBox )
        {
            position =  theMomentResult.center;
        }
    };

    class MaximumCursor :public Cursor {
    public:
    MaximumCursor(const MaximumResults & theMaximumResult, const asl::Box2f & theBox)
        : Cursor( theBox )
        {
            position = theMaximumResult.center;
        }
    };

    typedef std::map<int, Cursor> CursorMap;
}

#endif // Y60_ASS_CURSOR
