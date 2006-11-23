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