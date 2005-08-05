//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: NullSource.h,v $
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

#ifndef INCL_NULLSOURCE
#define INCL_NULLSOURCE

#include "AudioAsyncModule.h"

namespace AudioBase {

class AudioBuffer;
class AudioOutput;

class NullSource: public AudioAsyncModule {
public:
    NullSource(int mySampleRate);
    virtual ~NullSource();
    void disconnect();
    virtual void process();
    virtual bool init();

    AudioOutput * getOutput() {
        return _myOutput;
    }

private:
    NullSource(const NullSource&);
    AudioOutput* _myOutput;
    AudioBuffer* _myBuffer;
};

}

#endif
