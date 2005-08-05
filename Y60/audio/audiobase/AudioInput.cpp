//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioInput.cpp,v $
//     $Author: pavel $
//   $Revision: 1.6 $
//
//=============================================================================

#include "AudioInput.h"
#include "AudioSyncModule.h"

#include <asl/Logger.h>

#include <iostream>

using namespace std;

namespace AudioBase {

AudioInput::AudioInput(AudioSyncModule& myModule, ChannelType myType)
    : _myModule(myModule),
      _isConnected(false),
      _myType(myType)
{
    AC_DEBUG << "AudioInput on module=" << myModule.getName();
    _myData = 0;
}

AudioInput::~AudioInput()
{
    AC_DEBUG << "~AudioInput() on module=" << _myModule.getName();
    setConnected(false);
}

void
AudioInput::put(AudioBuffer& theData) {
    _myData = &theData;
    _myModule.nextInputDelivered();
}

AudioBuffer*
AudioInput::get() {
    return _myData;
}

AudioSyncModule&
AudioInput::getModule() {
    return _myModule;
}

void
AudioInput::stop() {
    AC_DEBUG << "stop() disc. input on module=" << _myModule.getName();
    _myModule.inputDisconnected(this);
    setConnected(false);
}

void
AudioInput::setConnected(bool theConnectFlag) {
    if (theConnectFlag == _isConnected) {
        AC_DEBUG << "setConnected() already " << (_isConnected ? "" : "dis") << "connected";
    }
    _isConnected = theConnectFlag;
}

bool
AudioInput::isConnected () {
    return _isConnected;
}
}
