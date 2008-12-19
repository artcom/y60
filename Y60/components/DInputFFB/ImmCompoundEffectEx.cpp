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

#include "ImmCompoundEffectEx.h"

#include <asl/math/numeric_functions.h>

#include <assert.h>

CImmCompoundEffectEx::CImmCompoundEffectEx()
:   CImmCompoundEffect( NULL )
{
    assert(0); // Never call (cast instead)
}

BOOL CImmCompoundEffectEx::Start( DWORD dwIterations, DWORD dwFlags)
{
    return BOOL
    (   IsBeyondStartDelay()
    &&  CImmCompoundEffect::Start( dwIterations, dwFlags )
    );
}

// -----------------------------------
// CImmCompoundEffectEx::GetStartDelay
// -----------------------------------
// Determines the shortest start delay.
//
BOOL CImmCompoundEffectEx::GetStartDelay( DWORD &StartDelay )
{
    StartDelay = MAXDWORD;
    BOOL result = TRUE;

    for
    (   int i = 0, max = GetNumberOfContainedEffects()
    ;   i < max
    ;   i++
    ){
        DWORD CurrentStartDelay;
        CImmEffect* pIE = GetContainedEffect( i );
        if ( pIE && pIE->GetStartDelay( CurrentStartDelay )	)
            StartDelay = asl::minimum( StartDelay, CurrentStartDelay );
        else
            result = FALSE;
    }

    return BOOL
    (   result 
    &&  max > 0
    );
}

// ---------------------------------
// CImmCompoundEffectEx::GetDelayEnd
// ---------------------------------
// Computes end of earliest start delay. Compare this value with ::GetTickCount()
// to determine if any component waveform started playing on the device.
//
BOOL CImmCompoundEffectEx::GetDelayEnd( DWORD &DelayEnd )
{
    DelayEnd = MAXDWORD;
    BOOL result = TRUE;

    for
    (   int i = 0, max = GetNumberOfContainedEffects()
    ;   i < max
    ;   i++
    ){
        DWORD StartDelay;
        CImmEffect* pIE = GetContainedEffect( i );
        if ( pIE && pIE->GetStartDelay( StartDelay ) )
            DelayEnd = asl::minimum( DelayEnd, StartDelay + pIE->m_dwLastStarted );
        else
            result = FALSE;
    }

    return BOOL
    (	result 
    &&	max > 0
    );
}

// ------------------------------------
// CImmCompoundEffectEx::ChangeDuration
// ------------------------------------
// Analogous to CImmEffect::ChangeDuration. Changes duration of all component effects.
// Returns false if any effect returns false. Attempts to change duration of all effects
// regardless of individual return values.
//
BOOL CImmCompoundEffectEx::ChangeDuration( DWORD Duration )
{
    DWORD CurrentDuration;
    BOOL result = GetDuration( CurrentDuration );

    if ( result )
    {
        DWORD RelativeDuration = Duration - CurrentDuration;

        for
        (   int i = 0, max = GetNumberOfContainedEffects()
        ;   i < max
        ;   i++
        ){
            IMM_ENVELOPE Envelope = {0};
            CImmEffect* pIE = GetContainedEffect( i );
            result &= BOOL
            (   pIE
            &&  pIE->GetDuration( CurrentDuration )
            &&  pIE->ChangeDuration( CurrentDuration + RelativeDuration )
            &&  (   !pIE->GetEnvelope( &Envelope )
                    ||  (   Envelope.dwAttackTime = ( CurrentDuration ? (DWORD)((float)Envelope.dwAttackTime * (float)Duration / (float)CurrentDuration) : 0 )
                        ,   Envelope.dwFadeTime = ( CurrentDuration ? (DWORD)((float)Envelope.dwFadeTime * (float)Duration / (float)CurrentDuration) : 0 )
                        ,   pIE->ChangeEnvelope( &Envelope )
                    )
                )
            );
        }

        result &= BOOL( max > 0 );
    }

    return result;
}

// --------------------------------
// CImmCompoundEffectEx::ChangeGain
// --------------------------------
// Analogous to CImmEffect::ChangeGain. Changes gain of all component effects.
// Returns false if any effect returns false. Attempts to change gain of all effects
// regardless of individual return values.
//
BOOL CImmCompoundEffectEx::ChangeGain( DWORD Gain )
{
    DWORD CurrentGain;
    BOOL result = GetGain( CurrentGain );

    if ( result )
    {
        DWORD RelativeGain = Gain - CurrentGain;

        for
        (   int i = 0, max = GetNumberOfContainedEffects()
        ;   i < max
        ;   i++
        ){
            CImmEffect* pIE = GetContainedEffect( i );
            result &= BOOL
            (   pIE
            &&  pIE->GetGain( CurrentGain )
            &&  pIE->ChangeGain( CurrentGain + RelativeGain )
            );
        }

        result &= BOOL( max > 0 );
    }

    return result;
}

// -------------------------------
// CImmCompoundEffectEx::GetStatus
// -------------------------------
// Analogous to CImmEffect::GetStatus. ORs all status flags from all component effects.
// Returns false if any effect returns false. Attempts to get status of all effects
// regardless of individual return values.
//
BOOL CImmCompoundEffectEx::GetStatus( DWORD &Status )
{
    Status = 0;
    BOOL result = TRUE;

    for
    (   int i = 0, max = GetNumberOfContainedEffects()
    ;   i < max
    ;   i++
    ){
        DWORD CurrentStatus;
        CImmEffect* pIE = GetContainedEffect( i );
        if ( pIE && pIE->GetStatus( &CurrentStatus ) )
            Status |= CurrentStatus;
        else
            result = FALSE;
    }

    return BOOL
    (   result
    &&  max > 0
    );
}

BOOL CImmCompoundEffectEx::ChangeStartDelay( DWORD StartDelay )
{
    DWORD CurrentStartDelay;
    BOOL result = GetStartDelay( CurrentStartDelay );

    if ( result )
    {
        DWORD RelativeStartDelay = StartDelay - CurrentStartDelay;
		
        for
        (   int i = 0, max = GetNumberOfContainedEffects()
        ;   i < max
        ;   i++
        ){
            CImmEffect* pIE = GetContainedEffect( i );
            result &= BOOL
            (   pIE
            &&  pIE->GetStartDelay( CurrentStartDelay )
            &&  pIE->ChangeStartDelay( CurrentStartDelay + RelativeStartDelay )
            );
        }

        result &= BOOL( max > 0 );
    }

    return result;
}

BOOL CImmCompoundEffectEx::GetDuration( DWORD &Duration )
{
    Duration = 0;
    BOOL result = TRUE;

    for
    (   int i = 0, max = GetNumberOfContainedEffects()
    ;   i < max
    ;   i++
    ){
        DWORD CurrentDuration;
        CImmEffect* pIE = GetContainedEffect( i );
        if ( pIE && pIE->GetDuration( CurrentDuration )	)
            Duration = asl::maximum( Duration, CurrentDuration );
        else
            result = FALSE;
    }

    return BOOL
    (   result
    &&  max > 0
    );
}

BOOL CImmCompoundEffectEx::GetDirection( LONG &DirectionX, LONG &DirectionY )
{
    CImmEffect* pEffect;

	return BOOL
	(	GetNumberOfContainedEffects() > 0
	&&	(pEffect = GetContainedEffect((long)0))
	&&	pEffect->GetDirection( DirectionX, DirectionY )
	);
}

BOOL CImmCompoundEffectEx::GetDirection( LONG &Angle )
{
    CImmEffect* pEffect;

	return BOOL
	(	GetNumberOfContainedEffects() > 0
	&&	(pEffect = GetContainedEffect((long)0))
	&&	pEffect->GetDirection( Angle )
	);
}

BOOL CImmCompoundEffectEx::ChangeDirection( LONG DirectionX, LONG DirectionY )
{
	if ( DirectionX == 0 && DirectionY == 0 )
		return FALSE;

	if ( DirectionX == 0 )
		return ChangeDirection( (DirectionY > 0 ? 9000 : 27000) );

	if ( DirectionY == 0 )
		return ChangeDirection( (DirectionX > 0 ? 18000 : 0) );

	return ChangeDirection( (LONG)atan2( (double)DirectionX, (double)DirectionY ) );
}

BOOL CImmCompoundEffectEx::ChangeDirection( LONG Angle )
{
    BOOL result = TRUE;
	int max = GetNumberOfContainedEffects();

	if (max > 0)
	{
		LONG CurrentAngle, RelativeAngle;
		CImmEffect* pIE = GetContainedEffect((long)0);
		pIE->GetDirection( CurrentAngle );
		RelativeAngle = Angle - CurrentAngle;
		pIE->ChangeDirection( Angle );

		for
		(   int i = 1
		;   i < max
		;   i++
		){
			pIE = GetContainedEffect( i );

			result &= BOOL
			(	pIE
			&&	pIE->GetDirection( CurrentAngle )
			&&	pIE->ChangeDirection( CurrentAngle + RelativeAngle )
			);
		}
	}

    return BOOL
    (   result
    &&  max > 0
    );
}

BOOL CImmCompoundEffectEx::GetGain( DWORD &Gain )
{
    Gain = 0;
    BOOL result = TRUE;

    for
    (   int i = 0, max = GetNumberOfContainedEffects()
    ;   i < max
    ;   i++
    ){
        DWORD CurrentGain;
        CImmEffect* pIE = GetContainedEffect( i );
        if ( pIE && pIE->GetGain( CurrentGain )	)
            Gain = asl::maximum( Gain, CurrentGain );
        else
            result = FALSE;
    }

    return BOOL
    (   result
    &&  max > 0
    );
}
