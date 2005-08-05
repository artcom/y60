//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioOutput.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.10 $
//
//=============================================================================

#include "AudioOutput.h"
#include "AudioInput.h"
#include "AudioModule.h"
#include "AudioSyncModule.h"
#include "AudioScheduler.h"
#include "AudioBuffer.h"
#include "SoundException.h"
#include "VolumeFader.h"

#include <asl/Assure.h>
#include <asl/Logger.h>

#include <stdio.h>

using namespace std;  

#define DB(x) // x

namespace AudioBase {

AudioOutput::AudioOutput(ChannelType myType, double myVolume, 
            VolumeFader * myFader)
    : _myType (myType),
      _myConnectedInput(0),
      _curSample(0),
      _myFader (myFader)
{
}

AudioOutput::~AudioOutput() {
    if (_myConnectedInput) {
        AC_DEBUG << "~AudioOutput() disc. input='" << _myConnectedInput->getModule().getName() << "'";
        _myConnectedInput->setConnected(false);
        _myConnectedInput = 0;
    }
}

void 
AudioOutput::put(AudioBuffer& theData) {
    /* XXX
    if (!testChannelCompatibility(theData.getType(), _myType)) {
        AC_WARNING("AudioOutput::put(): Left/Right mismatch between Buffer data" 
                   << endl << "type and AudioOutput data type.");
    }
    */
    if (_myConnectedInput) {
        _myFader->applyVolume(_curSample, theData);
        _myConnectedInput->put(theData);
        _curSample += AudioBuffer::getNumSamples();
    } else {
        AC_WARNING << "put() unconnected output sending data";
    }
}

void
AudioOutput::connect(AudioInput* theInput) {
    if (_myConnectedInput) {
        throw SoundException(string("AudioOutput::connect error: can't connect twice!\n") +
                 "Previous Input: " + _myConnectedInput->getModule().getName() +"\n"+
                 "New Input: " + theInput->getModule().getName(),PLUS_FILE_LINE);
    }
    ASSURE(testChannelCompatibility(theInput->getType(), _myType));

    _myConnectedInput = theInput;
    _myConnectedInput->setConnected();
}

AudioModule* 
AudioOutput::getInputModule() {
    return (_myConnectedInput ? &(_myConnectedInput->getModule()) : 0);
}

void 
AudioOutput::stop() {    
    if (_myConnectedInput) {
        AC_DEBUG << "stop() input='" << _myConnectedInput->getModule().getName() << "'";
        _myConnectedInput->stop();
        _myConnectedInput = 0;
    } else {
        AC_WARNING << "stop() no connected input";
    }
}

}
