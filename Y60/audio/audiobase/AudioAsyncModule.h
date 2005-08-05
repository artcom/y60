//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioAsyncModule.h,v $
//
//     $Author: valentin $
//
//   $Revision: 1.7 $
//
//
// Description:
//
//
//=============================================================================

#ifndef INCL_AUDIOASYNCMODULE
#define INCL_AUDIOASYNCMODULE

#include "AudioModule.h"

namespace AudioBase {

class AudioAsyncModule: public AudioModule {
public:
    AudioAsyncModule(const std::string & myName, int mySampleRate,
            double myVolume, bool myFadeIn)
        : AudioModule (myName, mySampleRate, myVolume, myFadeIn)
        {}
    virtual ~AudioAsyncModule() {}

    virtual bool init();
    virtual void pause();
    virtual void stop();
    virtual void setSeekOffset(double theSeekOffset);
    void go();

protected:
};

typedef std::list<AudioAsyncModule*> AsyncModuleList;

}

#endif
