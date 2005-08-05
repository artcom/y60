//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: Lowpass.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.3 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_LOWPASS
#define INCL_LOWPASS

#include "AudioSyncModule.h"

namespace AudioBase {

class AudioInput;
class AudioBuffer;

class Lowpass: public AudioSyncModule {
public:
    Lowpass(int myFrequency, int mySampleRate, double myVolume);
    virtual ~Lowpass();
    virtual void process();

private:
    AudioBuffer * _outBuffer;
    AudioBuffer * _lastBuffer;
    int _myFrequency;
    int _myFilterSize;
    int _curSample;
    SAMPLE _curAvg;
    bool _isFirstBuffer;
};

}

#endif


