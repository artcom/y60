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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ImmCompoundEffectEx_H_
#define _ImmCompoundEffectEx_H_

#include <IFC/ifc.h>

// --------------------
// CImmCompoundEffectEx
// --------------------
// CImmCompoundEffectEx makes CImmEffect member functions available
// to a group of effects. It attempts to preserve relative differences
// between each component effect.
//
// Do not instantiate. (Do not call constructor)
// Instead, cast existing CImmCompoundEffect* to CImmCompoundEffectEx*
// Utility functions are specific to the needs of this system.
//
class CImmCompoundEffectEx : public CImmCompoundEffect
{
public:
    // dummy constructor
    CImmCompoundEffectEx(); // Never call (cast instead)

    // CImmEffect extensions
    BOOL GetStatus( DWORD &Status );
    BOOL GetStartDelay( DWORD &StartDelay );
    BOOL GetDuration( DWORD &Duration );
    BOOL GetGain( DWORD &Gain );
	BOOL GetDirection( LONG &DirectionX, LONG &DirectionY );
	BOOL GetDirection( LONG &Angle );
    BOOL ChangeDuration( DWORD Duration );
    BOOL ChangeGain( DWORD Gain );
    BOOL ChangeStartDelay( DWORD StartDelay );
	BOOL ChangeDirection( LONG DirectionX, LONG DirectionY );
	BOOL ChangeDirection( LONG Angle );

    // CImmCompoundEffect overrides
    BOOL Start( DWORD dwIterations = IMM_EFFECT_DONT_CHANGE, DWORD dwFlags = 0 );

    // utility functions
    BOOL GetDelayEnd( DWORD &DelayEnd );
    BOOL IsBeyondStartDelay()
    {
        DWORD DelayEnd;
        return BOOL
        (   GetDelayEnd( DelayEnd )
        &&  DelayEnd < ::GetTickCount()		// Does not account for counter overflow.
        );
    }
    BOOL IsPlaying()
    {
        DWORD Status;
        return BOOL( GetStatus( Status ) && (Status & IMM_STATUS_PLAYING) );
    }
};

#endif // _ImmCompoundEffectEx_H_