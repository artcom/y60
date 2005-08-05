//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: WinWaveBuffer.cpp,v $
//
//     $Author: christian $
//
//   $Revision: 1.2 $
//
//
// Description:
//
//
//=============================================================================

#include "WinMMSoundCard.h"

namespace AudioBase {

using namespace std;
using namespace asl;

WinWaveBuffer::WinWaveBuffer(unsigned int mySize, DWORD myUserData) {
    _myBuffer = new char[mySize];
    _myHeader.dwBufferLength = mySize;
    _myHeader.dwFlags = 0;
    _myHeader.lpData = _myBuffer;
    _myHeader.dwUser = myUserData;
}

WinWaveBuffer::~WinWaveBuffer() {
    delete[] _myBuffer;
}

char * 
WinWaveBuffer::getBuffer() {
    return _myBuffer;
}

WAVEHDR *
WinWaveBuffer::getWavHeader() {
    return &_myHeader;
}

void 
WinWaveBuffer::clear() {
    memset(_myBuffer, 0, _myHeader.dwBufferLength);
}

}
