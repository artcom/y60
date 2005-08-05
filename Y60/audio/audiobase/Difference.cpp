//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: Difference.cpp,v $
//
//     $Author: christian $
//
//   $Revision: 1.4 $
//
//
// Description: 
//
//
//=============================================================================

#include "Difference.h"
#include "AudioScheduler.h"
#include "AudioSyncModule.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include "AudioBuffer.h"

#include <asl/Time.h>


using namespace std;  // automatically added!



namespace AudioBase {

Difference::Difference(int mySampleRate, double myVolume)
        : AudioSyncModule("Difference", 2, mySampleRate, myVolume),
          _outBuffer(0)
{ 
    newOutput(Mono);
}

Difference::~Difference() { 
    delete _outBuffer;
}

void Difference::process () {
    if (!_outBuffer) {
        _outBuffer = new AudioBuffer();
    }
    SAMPLE * inBuffer0 = getInput(0)->get()->getData();
    SAMPLE * inBuffer1 = getInput(1)->get()->getData();
    SAMPLE * outBuffer = _outBuffer->getData();
    unsigned myBufferSize = AudioBuffer::getNumSamples();
    for (int i=0; i < myBufferSize; i++) {
        outBuffer[i] = inBuffer0[i]-inBuffer1[i];
    }

    getOutput(0)->put(*_outBuffer);
    resetDeliveredInputs();
}

}





