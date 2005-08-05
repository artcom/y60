//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SineGenerator.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.6 $
//
//
// Description:
//
//
//=============================================================================

#ifndef INCL_SINEGENERATOR
#define INCL_SINEGENERATOR

#include "AudioAsyncModule.h"

namespace AudioBase {

class AudioOutput;
class AudioBuffer;

class SineGenerator: public AudioAsyncModule {
public:
    SineGenerator(int mySampleRate);  // Duration in seconds.
    virtual ~SineGenerator();
    virtual void process();
    int getBuffersGenerated() { return _myBuffersGenerated; }
    virtual bool init();

    AudioOutput * _myOutput;

private:
    double getSoftSin(int theSampleNum);
    int _curSample;
    int _myBuffersGenerated;
    AudioBuffer * _myBuffer;
};

}

#endif
