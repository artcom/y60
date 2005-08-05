//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: TFilter.h,v $
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

#ifndef INCL_TFILTER
#define INCL_TFILTER

#include "AudioSyncModule.h"

namespace AudioBase {

class AudioBuffer;
class AudioInput;

class TFilter: public AudioSyncModule {
public:
    TFilter(int numChannels, int mySampleRate, double myVolume = 1.0);
    virtual ~TFilter();
    virtual void process ();

private:
};

}

#endif


