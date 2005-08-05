//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: Difference.h,v $
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

#ifndef INCL_DIFFERENCE
#define INCL_DIFFERENCE

#include "AudioSyncModule.h"

namespace AudioBase {

class AudioInput;
class AudioBuffer;

class Difference: public AudioSyncModule {
public:
    Difference(int mySampleRate, double myVolume=1.0);
    virtual ~Difference();
    virtual void process();

private:
    AudioBuffer * _outBuffer;
};

}

#endif


