//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioEater.h,v $
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

#ifndef INCL_AUDIOEATER
#define INCL_AUDIOEATER

#include "AudioSyncModule.h"

namespace AudioBase {

class AudioBuffer;
class AudioInput;

class AudioEater: public AudioSyncModule {
public:
    AudioEater(int numChannels, int mySampleRate);
    virtual ~AudioEater();
    virtual void process ();
    int getBuffersEaten();
    double getMaxVolume();

private:
    int _numBuffersEaten;
    double _myMaxVolume;
};

}

#endif


