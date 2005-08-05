//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SoundOut.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.2 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_SOUNDOUT
#define INCL_SOUNDOUT

#include "AudioSyncModule.h"
#include "VolumeFader.h"

namespace AudioBase {
    class SoundCard;

class SoundOut: public AudioSyncModule {
public:
    SoundOut (SoundCard* theSoundCard, int numChannels, 
            int mySampleRate, double myVolume=1.0);
    virtual ~SoundOut ();

    virtual void process ();

    virtual void outputDeviceStats () const;
    virtual void inputDisconnected(AudioInput* theInput);
    bool hasClicks();

private:
    void initDevice ();
    void initTimer ();

    double getSampleTime ();

    SoundCard* _mySoundCard;

    int _myNumClicks;
    double _myMaxVolume;
};

}
#endif
