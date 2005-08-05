//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: Lowpass.cpp,v $
//
//     $Author: christian $
//
//   $Revision: 1.5 $
//
//
// Description: 
//
//
//=============================================================================

#include "Lowpass.h"
#include "AudioSyncModule.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include "AudioBuffer.h"

#include <asl/Time.h>

using namespace std;

namespace AudioBase {

Lowpass::Lowpass(int myFrequency, int mySampleRate, double myVolume)
        : AudioSyncModule("Lowpass", 1, mySampleRate, myVolume),
          _myFrequency (myFrequency),
          _curAvg (0.0),
          _isFirstBuffer(true),
          _lastBuffer(0),
          _outBuffer(0)
{ 
    _myFilterSize = mySampleRate/myFrequency;
    _curSample = 0;
    newOutput(Mono);
}

Lowpass::~Lowpass() { 
    delete _lastBuffer;
    delete _outBuffer;
}

void Lowpass::process () {
    if (!_lastBuffer) {
        _lastBuffer = new AudioBuffer();
        _outBuffer = new AudioBuffer();
    }
    SAMPLE * curBuffer = getInput(0)->get()->getData();
    SAMPLE * lastBuffer = _lastBuffer->getData();
    SAMPLE * outBuffer = _outBuffer->getData();
    for (int i=0; i<_myFilterSize; i++) {
        _curAvg += curBuffer[i];
        if (!_isFirstBuffer) {
            _curAvg -= lastBuffer[AudioBuffer::getNumSamples()+i-_myFilterSize];
        }
        outBuffer[i] = _curAvg/_myFilterSize;
    }

    unsigned myBufferSize = AudioBuffer::getNumSamples();
    for (int i=_myFilterSize; i<myBufferSize; i++) {
        _curAvg += curBuffer[i];
        _curAvg -= curBuffer[i-_myFilterSize];
        outBuffer[i] = _curAvg/_myFilterSize;
    }

    getOutput(0)->put(*_outBuffer);
    resetDeliveredInputs();

    _lastBuffer->setData(curBuffer);
    _isFirstBuffer = false;
}

}





