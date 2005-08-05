
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: NullSource.cpp,v $
//
//     $Author: thomas $
//
//   $Revision: 1.7 $
//
//
// Description:
//
//
//=============================================================================

#include "NullSource.h"
#include "AudioScheduler.h"
#include "AudioOutput.h"
#include "AudioBuffer.h"

namespace AudioBase {

NullSource::NullSource(int mySampleRate)
        : AudioAsyncModule ("NullSource", mySampleRate, 1.0, false)
{
    _myOutput = newOutput(Mono);
}

NullSource::~NullSource() {
    if (_myOutput) {
        _myOutput->stop();
    }
}

void 
NullSource::disconnect() {
    deleteOutput(_myOutput);
    _myOutput = 0;
}

bool NullSource::init() {
    _myBuffer = new AudioBuffer();
    _myBuffer->clear();
    go();
    return true;
}

void
NullSource::process() {
    _myOutput->put(*_myBuffer);
}


}
