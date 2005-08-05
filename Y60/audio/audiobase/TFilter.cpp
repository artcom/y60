//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: TFilter.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.3 $
//
//
// Description: 
//
//
//=============================================================================

#include "TFilter.h"
#include "AudioSyncModule.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include "AudioBuffer.h"

#include <asl/Logger.h>
#include <asl/Time.h>


using namespace std;  

namespace AudioBase {

TFilter::TFilter(int numChannels, int mySampleRate, double myVolume)
        : AudioSyncModule("TFilter", 1, mySampleRate, myVolume)
{ 
    for (int i=0; i<numChannels; i++) {
        newOutput(Mono);
    }
}

TFilter::~TFilter() { 
}

void TFilter::process () {
    for (int i=0; i<getNumOutputs(); i++) {
//        AC_DB(getInput(0)->get()->getMax());
        getOutput(i)->put(*getInput(0)->get());
    }

    resetDeliveredInputs();
}

}





