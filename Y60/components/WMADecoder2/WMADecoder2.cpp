//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "WMADecoder2.h"
#include "WMHelper.h"

#include <asl/Auto.h>
#include <asl/Pump.h>
#include <asl/string_functions.h>

#include <atlbase.h> // For CComPtr and CComQIPtr
#include <dsound.h>  // For the DSSPEAKER_5POINT1 value
#include <mmreg.h>   // For WAVEFORMATEXTENSIBLE (if you use it)

#define DB(x) // x

using namespace std;
using namespace asl;

namespace y60 {

WMADecoder2::WMADecoder2 (string myURI)
    : _myURI (myURI)
{
    AC_DEBUG << "WMADecoder2::WMADecoder2";
    _myEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!_myEvent) {
        throw DecoderException("Create Event failed", PLUS_FILE_LINE);
    }
    open();
}

WMADecoder2::~WMADecoder2()
{
    AC_DEBUG << "WMADecoder2::~WMADecoder2 (" << _myURI << ")";
    if (_myEvent) {
        CloseHandle(_myEvent);
    }

    close();
}

std::string WMADecoder2::getName() const {
    return _myURI;
}

Time WMADecoder2::getDuration() const {
    return 0.0;
}

void WMADecoder2::seek (Time thePosition)
{
}

void WMADecoder2::open() {
    AC_DEBUG << "WMADecoder2::open (" << _myURI << ")" << _myURI;
    ResetEvent(_myEvent);
    HRESULT hr = WMCreateReader(NULL, WMT_RIGHT_PLAYBACK, &_myReader);
    checkForWMError(hr, "Could not create reader", PLUS_FILE_LINE);

    // Convert string to wstring
    size_t mySizeNeeded = ::mbstowcs(0, _myURI.c_str(), _myURI.length());
    std::wstring myWideUrl;
    myWideUrl.resize(mySizeNeeded);
    ::mbstowcs(&myWideUrl[0], _myURI.c_str(), _myURI.length());

    // Open file
    hr = _myReader->Open(myWideUrl.c_str(), this, 0);
    checkForWMError(hr, string("1+Could not open file: ") + _myURI, PLUS_FILE_LINE);
    waitForEvent();
    checkForWMError(_myEventResult, string("2+Could not open file: ") + _myURI, PLUS_FILE_LINE);
/*
    if (_myMaxChannels > 2) {
        hr = setupMultiChannel();
        checkForWMError(hr, string("2+Could not setup Multichannel for file: ") + _myURI, PLUS_FILE_LINE);
    }
*/
//    setupAudio(_myURI);

    // Start playing
    hr = _myReader->Start(QWORD(0), 0, 1, NULL);
    checkForWMError(hr, "1+Could not start reader", PLUS_FILE_LINE);
    waitForEvent();
    checkForWMError(_myEventResult, "2+Could not start reader", PLUS_FILE_LINE);

    DB(cerr << "+++ Open succeeded +++" << endl);
}

void WMADecoder2::close() {
    AC_DEBUG << "WMADecoder2::close() (" << _myURI << ")";
    if (_myReader) {
        _myReader->Release();
        _myReader = NULL;
    }
}

bool WMADecoder2::waitForEvent(unsigned theWaitTime) {
    DWORD myResult = WaitForSingleObject(_myEvent, theWaitTime);
    if (myResult == WAIT_OBJECT_0) {
        // The event handle was set to signaled state
        return true;
    } else if (myResult == WAIT_TIMEOUT) {
        //throw WindowsMediaAudioException("Time out waiting for the event.", PLUS_FILE_LINE);
        cerr << "### WARNING : WMADecoder timeout waiting for event" << endl;
        return false;
    } else if (myResult == WAIT_FAILED) {
        throw DecoderException("WaitForSingleObject failed.", PLUS_FILE_LINE);
    }
    return false;
}

    
bool WMADecoder2::decode(asl::ISampleSink* mySampleSink) {
    return false;
}

unsigned WMADecoder2::getSampleRate() {
    return 0;
}

unsigned WMADecoder2::getNumChannels() {
    return 0;
}


}

