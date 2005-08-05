//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SoundIn.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.4 $
//
//
// Description:
//
//
//=============================================================================

#ifndef INCL_SOUNDIN
#define INCL_SOUNDIN

#include "asl/ITimeSource.h"
#include "AudioAsyncModule.h"
#include <string>

namespace AudioBase {

    class AudioBuffer;
    class SoundCard;

class SoundIn: public AudioAsyncModule {
public:
    SoundIn(SoundCard* theSoundCard, int myNumChannels, int mySampleRate, double myVolume);
    virtual ~SoundIn();

    virtual bool init();
    virtual void process ();

    virtual int getNumOverruns() const;
protected:
    AudioBuffer * getBuffer(int i);
    asl::Signed64     _myReadBlocks;

private:
    SoundCard* _mySoundCard;
    int _myNumUnderruns;

    std::vector <AudioBuffer* > _myBuffer;
};

}
#endif
