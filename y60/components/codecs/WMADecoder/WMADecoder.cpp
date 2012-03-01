/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "WMADecoder.h"
#include "WMHelper.h"

#include <asl/base/Auto.h>
#include <asl/audio/Pump.h>
#include <asl/base/string_functions.h>

#include <atlbase.h> // For CComPtr and CComQIPtr
#include <dsound.h>  // For the DSSPEAKER_5POINT1 value
#include <mmreg.h>   // For WAVEFORMATEXTENSIBLE (if you use it)

#define DB(x) x

using namespace std;
using namespace asl;

namespace y60 {

WMADecoder::WMADecoder (const string& myURI)
    : _myURI (myURI),
      _mySampleSink(0),
      _myReader(0),
      _myDecodingDone(false),
      _myResampleContext(0),
      _myResampledSamples(AVCODEC_MAX_AUDIO_FRAME_SIZE),
      _myState(STOPPED),
      _myEvent(0),
      _mySampleEvent(0),
      _myCurFrame(0)
{
    try {
        AC_DEBUG << "WMADecoder::WMADecoder";
        _myEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (!_myEvent) {
            throw DecoderException("Create Event failed", PLUS_FILE_LINE);
        }
        _mySampleEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (!_mySampleEvent) {
            throw DecoderException("Create Event failed", PLUS_FILE_LINE);
        }
        open();
    } catch (...) {
        AC_DEBUG << "WMADecoder::WMADecoder: Exception";
        if (_myEvent) {
            CloseHandle(_myEvent);
        }
        if (_mySampleEvent) {
            CloseHandle(_mySampleEvent);
        }
        close();
        throw;
    }
}

WMADecoder::~WMADecoder()
{
    AC_DEBUG << "WMADecoder::~WMADecoder (" << _myURI << ")";
    if (_myEvent) {
        CloseHandle(_myEvent);
    }
    if (_mySampleEvent) {
        CloseHandle(_mySampleEvent);
    }
    close();
}

void WMADecoder::play() {
    AC_DEBUG << "WMADecoder::play";
    if (_myState != PLAYING) {
        _myState = PLAYING;
        HRESULT hr = _myReader->Start(QWORD(0), 0, 1, NULL);
        checkForWMError(hr, "1+Could not start reader", PLUS_FILE_LINE);
        waitForEvent();
        checkForWMError(_myEventResult, "2+Could not start reader", PLUS_FILE_LINE);
        _myDecodingDone = false;
        waitForSamples();
    }
}

void WMADecoder::stop() {
    AC_DEBUG << "WMADecoder::stop";
    _myState = STOPPED;
    HRESULT hr = _myReader->Pause();
    checkForWMError(hr, "WMADecoder::stop: Pause failed", PLUS_FILE_LINE);
    preroll(0);  // Seeks to 0 so we can call play() again without delay :-).
}

void WMADecoder::pause() {
    AC_DEBUG << "WMADecoder::pause";
    _myState = PAUSED;
    HRESULT hr = _myReader->Pause();
    checkForWMError(hr, "WMADecoder::stop: Pause failed", PLUS_FILE_LINE);
}

bool WMADecoder::isEOF() const {
    return _myDecodingDone;
}

void WMADecoder::seek (Time thePosition) {
    AC_DEBUG << "WMADecoder::seek";

    if (_myState == PLAYING) {
        // TODO: Does this work?
        HRESULT hr = _myReader->Start(QWORD(thePosition*1000*1000*10), 0, 1, NULL);
        checkForWMError(hr, "WMADecoder::seek: Could not start reader", PLUS_FILE_LINE);
    } else {
        preroll(thePosition);
        _myDecodingDone = false;
    }
}

unsigned WMADecoder::getSampleRate() {
    return _mySampleRate;
}

unsigned WMADecoder::getNumChannels() {
    return _myNumChannels;
}

Time WMADecoder::getDuration() const {
    return 0.0;
}

std::string WMADecoder::getName() const {
    return _myURI;
}

void WMADecoder::setSampleSink(asl::ISampleSink* mySampleSink) {
    AC_DEBUG << "setSampleSink";
    _mySampleSink = mySampleSink;
}

unsigned WMADecoder::getCurFrame() const {
    return _myCurFrame;
}

void WMADecoder::decodeEverything() {
    AC_DEBUG << "WMADecoder::decodeEverything";
    _myState = PLAYING;
    HRESULT hr = _myReader->Start(QWORD(0), 0, 1, NULL);
    checkForWMError(hr, "decodeEverything: Could not start reader", PLUS_FILE_LINE);

    // Decode at most 1 Hour of data, that's 700MB at 16 bit, 48kHz, stereo :-).
    setTime(QWORD(60)*60*1000*1000*10);
    while (_myState != STOPPED) {
        waitForEvent();
    }
    AC_DEBUG << "WMADecoder::decodeEverything end";
}

bool WMADecoder::isSyncDecoder() const {
    return false;
}

void WMADecoder::setTime(asl::Time myTime) {
    if (_myUseUserClock) {
        IWMReaderAdvanced * myAdvReader;
        HRESULT hr = _myReader->QueryInterface(IID_IWMReaderAdvanced,(void **) &myAdvReader);
        checkForWMError(hr, string("QueryInterface(IWMReaderAdvanced) failed for ") + _myURI,
                PLUS_FILE_LINE);

        // Additional one second of buffering.
        hr = myAdvReader->DeliverTime(QWORD((myTime+1.0)*1000*1000*10));
        checkForWMError(hr, string("DeliverTime failed for ") + _myURI,
                PLUS_FILE_LINE);
        myAdvReader->Release();
    }
}

HRESULT STDMETHODCALLTYPE
WMADecoder::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR * ppvObject) {
    if ((riid == IID_IWMReaderCallback) || (riid == IID_IUnknown)) {
        *ppvObject = static_cast<IWMReaderCallback *>(this);
        AddRef();
        return S_OK;
    }

    *ppvObject = NULL;
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE
WMADecoder::AddRef() {
    return InterlockedIncrement(&_myReferenceCount);
}

ULONG STDMETHODCALLTYPE
WMADecoder::Release() {
    if (InterlockedDecrement(&_myReferenceCount) == 0) {
        // This will not work with our smart pointers
        //delete this;
        return 0;
    }

    return _myReferenceCount;
}

HRESULT STDMETHODCALLTYPE WMADecoder::OnSample(DWORD theOutputNumber,
        QWORD theSampleTime, QWORD theSampleDuration, DWORD theFlags,
        INSSBuffer __RPC_FAR *theSample, void __RPC_FAR *theContext)
{
    if (theOutputNumber == _myAudioOutputId) {
// TODO: Check locking
//        asl::AutoLocker<BufferedSource> myLocker(*this);
        BYTE * myBuffer;
        DWORD myBufferLength;
        HRESULT hr = theSample->GetBufferAndLength(&myBuffer, &myBufferLength);
        checkForWMError(hr, "Could not get buffer from sample", PLUS_FILE_LINE);
        AudioBufferPtr myAudioBuffer;
        int numFrames = myBufferLength/(_myNumChannels*2);  // Assumes BitsPerSample is 16!
        if (_myResampleContext) {
            numFrames = audio_resample(_myResampleContext,
                    (int16_t*)(_myResampledSamples.begin()),
                    (int16_t*)(myBuffer),
                    numFrames);
            myAudioBuffer = Pump::get().createBuffer(numFrames);
            myAudioBuffer->convert(_myResampledSamples.begin(), SF_S16,
                    _myNumChannels);
        } else {
            myAudioBuffer = Pump::get().createBuffer(numFrames);
            myAudioBuffer->convert(myBuffer, SF_S16, _myNumChannels);
        }
        myAudioBuffer->setStartFrame(_myCurFrame);
        _myCurFrame += myAudioBuffer->getNumFrames();
        _mySampleSink->queueSamples(myAudioBuffer);
        SetEvent(_mySampleEvent);
//        AC_DEBUG << "OnSample, BufferLength: " << myBufferLength;
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE WMADecoder::OnStatus(WMT_STATUS theStatus,
        HRESULT hr, WMT_ATTR_DATATYPE theType, BYTE __RPC_FAR *theValue,
        void __RPC_FAR *theContext)
{
// TODO: Check locking
//    asl::AutoLocker<BufferedSource> myLocker(*this);
    switch (theStatus) {
        case WMT_OPENED:
            DB(cerr << ">>> Opened file. " << (void*)this << endl;)
            _myEventResult = hr;
            SetEvent(_myEvent);
            break;
        case WMT_STARTED:
            DB(cerr << ">>> Started. " << (void*)this << endl;)
            _myEventResult = hr;
            SetEvent(_myEvent);
            break;
        case WMT_STOPPED:
            DB(cerr << ">>> Stopped. " << (void*)this << endl;)
            _myEventResult = hr;
            SetEvent(_myEvent);
            break;
        case WMT_CLOSED:
            DB(cerr << ">>> Closed the file. " << (void*)this << endl;)
            _myEventResult = hr;
            SetEvent(_myEvent);
            break;
        case WMT_END_OF_STREAMING:
            DB(cerr << ">>> End of file." << endl;)
            _myEventResult = hr;
            SetEvent(_myEvent);
            _myDecodingDone = true;
            _myState = STOPPED;
            break;
        case WMT_EOF:
            DB(cerr << ">>> End of file." << endl;)
            _myEventResult = hr;
            SetEvent(_myEvent);
            _myDecodingDone = true;
            _myState = STOPPED;
            break;
    default:
        //DB(cerr << ">>> OnStatus " << theStatus << endl);
            break;
    }

    return S_OK;
}

void WMADecoder::open() {
    AC_DEBUG << "WMADecoder::open (" << _myURI << ")";
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
    checkForWMError(hr, string("Could not open file: ") + _myURI, PLUS_FILE_LINE);
    waitForEvent();
    checkForWMError(_myEventResult, string("Could not open file: ") + _myURI, PLUS_FILE_LINE);
/*
    if (_myMaxChannels > 2) {
        hr = setupMultiChannel();
        checkForWMError(hr, string("2+Could not setup Multichannel for file: ")
                + _myURI, PLUS_FILE_LINE);
    }
*/
    IWMReaderAdvanced * myAdvReader;
    hr = _myReader->QueryInterface(IID_IWMReaderAdvanced,(void **) &myAdvReader);
    checkForWMError(hr, string("QueryInterface(IWMReaderAdvanced) failed for ") + _myURI,
            PLUS_FILE_LINE);
    hr = myAdvReader->SetUserProvidedClock(true);
    myAdvReader->Release();
    if (hr == NS_E_INVALID_REQUEST) {
        AC_WARNING << "WMADecoder::open: Stream does not support user-provided clock.";
        _myUseUserClock = false;
    } else {
        checkForWMError(hr, string("QueryInterface(IWMReaderAdvanced) failed for ") + _myURI,
                PLUS_FILE_LINE);
        _myUseUserClock = true;
    }

    setupAudio();
    preroll(0);

    DB(cerr << "+++ Open succeeded +++" << endl);
}

void WMADecoder::close() {
    AC_DEBUG << "WMADecoder::close() (" << _myURI << ")";
    if (_myReader) {
        _myReader->Release();
        _myReader = NULL;
    }
    if (_myResampleContext) {
        audio_resample_close(_myResampleContext);
        _myResampleContext = 0;
    }
}

void
WMADecoder::setupAudio() {
    AC_DEBUG << "WMADecoder::setupAudio()";

    DWORD myOutputCount = 0;
    HRESULT hr = _myReader->GetOutputCount(&myOutputCount);
    checkForWMError(hr, "Could not get output count.", PLUS_FILE_LINE);

    _myAudioOutputId = static_cast<WORD>(-1);

    for (WORD i = 0; i < myOutputCount; ++i) {
        IWMOutputMediaProps * myOutputProperties = 0;
        HRESULT hr = _myReader->GetOutputProps(i, &myOutputProperties);
        checkForWMError(hr, string("Could not get output properties ") + asl::as_string(i) +
                " of " + asl::as_string(myOutputCount) +
                " from reader.", PLUS_FILE_LINE);

        // Get the media type structure
        DWORD myMediaTypeSize;
        hr = myOutputProperties->GetMediaType(NULL, &myMediaTypeSize);
        checkForWMError(hr, string("Could not mediatypesize of output ") +
                asl::as_string(i) + " from IWMOutputMediaProps", PLUS_FILE_LINE);
        WM_MEDIA_TYPE * myMediaType = (WM_MEDIA_TYPE *) new BYTE[myMediaTypeSize];
        hr = myOutputProperties->GetMediaType(myMediaType, &myMediaTypeSize);
        checkForWMError(hr, string("Could not mediatype of output ") +
                asl::as_string(i) + " from IWMOutputMediaProps", PLUS_FILE_LINE);

        GUID myMajorType = myMediaType->majortype;
        if (myMajorType == WMMEDIATYPE_Audio) {
            DB(cerr << "Output " << i << " is an audio stream" << endl;)
            _myAudioOutputId = i;
        } else if (myMajorType == WMMEDIATYPE_Script) {
            DB(cerr << "Output " << i << " is a script stream" << endl;)
        } else if (myMajorType == WMMEDIATYPE_FileTransfer) {
            DB(cerr << "Output " << i << " is a data stream" << endl;)
        } else if (myMajorType == WMMEDIATYPE_Image) {
            DB(cerr << "Output " << i << " is an image stream" << endl;)
        } else if (myMajorType == WMMEDIATYPE_Text) {
        DB(cerr << "Output " << i << " is a text stream" << endl;)
        }

        if (myMediaType->formattype == WMFORMAT_WaveFormatEx) {
            WAVEFORMATEX * myAudioInfo = (WAVEFORMATEX *)myMediaType->pbFormat;
            if (myAudioInfo->cbSize == 22) {
                cout << "extension" << endl;
            }
            _myNumChannels  = myAudioInfo->nChannels ;
            _mySampleRate = myAudioInfo->nSamplesPerSec;
            if (myAudioInfo->wBitsPerSample != 16) {
                throw DecoderException(_myURI+" has unsupported bits per sample "+
                        asl::as_string(myAudioInfo->wBitsPerSample), PLUS_FILE_LINE);
            }
            if (_mySampleRate != Pump::get().getNativeSampleRate()) {
#if  LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,15,0)
                _myResampleContext = av_audio_resample_init(
                    _myNumChannels, _myNumChannels,
                    Pump::get().getNativeSampleRate(), _mySampleRate,
                    SAMPLE_FMT_S16, SAMPLE_FMT_S16,
                    16, 10, 0, 0.8);
#else
                _myResampleContext = audio_resample_init(_myNumChannels,
                    _myNumChannels, Pump::get().getNativeSampleRate(),
                    _mySampleRate);
#endif
            }
        }

        if (myMediaType) {
            delete[] myMediaType;
        }

        if (myOutputProperties) {
            myOutputProperties->Release();
        }
    }

    if (_myAudioOutputId == -1) {
        throw DecoderException(std::string("No audio stream found"), PLUS_FILE_LINE);
    }

    AC_INFO << "  Number of channels: " << _myNumChannels;
    AC_INFO << "  Samples per second: " << _mySampleRate;
}

void WMADecoder::preroll(asl::Time thePosition) {
    AC_DEBUG << "WMADecoder::preroll(" << (double)thePosition << ")";
    ResetEvent(_mySampleEvent);
    IWMReaderAdvanced2 * myAdvReader;
    HRESULT hr = _myReader->QueryInterface(IID_IWMReaderAdvanced2,(void **) &myAdvReader);
    checkForWMError(hr, string("QueryInterface(IWMReaderAdvanced2) failed for ") + _myURI,
            PLUS_FILE_LINE);
    myAdvReader->Preroll(QWORD((double)thePosition*1000*1000*10), 0, 1);
    myAdvReader->Release();
}

void WMADecoder::waitForSamples() {
    DWORD myResult = WaitForSingleObject(_mySampleEvent, 1000);
    if (myResult == WAIT_OBJECT_0) {
        // The event handle was set to signaled state
        return;
    } else if (myResult == WAIT_TIMEOUT) {
        //throw WindowsMediaAudioException("Time out waiting for the event.", PLUS_FILE_LINE);
        AC_WARNING << "WMADecoder timeout waiting for samples";
    } else if (myResult == WAIT_FAILED) {
        throw DecoderException("WaitForSingleObject failed.", PLUS_FILE_LINE);
    }
    AC_WARNING << "WMADecoder::waitForSamples failed" ;

}

bool WMADecoder::waitForEvent(unsigned theWaitTime) {
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

}

