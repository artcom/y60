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

#define DB(x) x

using namespace std;
using namespace asl;

namespace y60 {

WMADecoder2::WMADecoder2 (const string& myURI)
    : _myURI (myURI),
      _mySampleSink(0),
      _myReader(0),
      _myDecodingDone(false),
      _myResampleContext(0),
      _myResampledSamples(AVCODEC_MAX_AUDIO_FRAME_SIZE),
      _myState(STOPPED)
{
    AC_DEBUG << "WMADecoder2::WMADecoder2";
    _myEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!_myEvent) {
        throw DecoderException("Create Event failed", PLUS_FILE_LINE);
    }
    _mySampleEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!_mySampleEvent) {
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
    if (_mySampleEvent) {
        CloseHandle(_mySampleEvent);
    }
    close();
}

void WMADecoder2::play() {
    AC_DEBUG << "WMADecoder2::play";
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

void WMADecoder2::stop() {
    AC_DEBUG << "WMADecoder2::stop";
    _myState = STOPPED;
    HRESULT hr = _myReader->Pause();
    checkForWMError(hr, "WMADecoder2::stop: Pause failed", PLUS_FILE_LINE);
    preroll(0);  // Seeks to 0 so we can call play() again without delay :-).
}

void WMADecoder2::pause() {
    AC_DEBUG << "WMADecoder2::pause";
    _myState = PAUSED;
    HRESULT hr = _myReader->Pause();
    checkForWMError(hr, "WMADecoder2::stop: Pause failed", PLUS_FILE_LINE);
}

bool WMADecoder2::isEOF() const {
    return _myDecodingDone;
}

void WMADecoder2::seek (Time thePosition) {
    AC_DEBUG << "WMADecoder2::seek";

    if (_myState == PLAYING) {
        // TODO: Does this work?
        HRESULT hr = _myReader->Start(QWORD(thePosition*1000*1000*10), 0, 1, NULL);
        checkForWMError(hr, "WMADecoder2::seek: Could not start reader", PLUS_FILE_LINE);
    } else {
        preroll(thePosition);
        _myDecodingDone = false;
    }
}

unsigned WMADecoder2::getSampleRate() {
    return _mySampleRate;
}

unsigned WMADecoder2::getNumChannels() {
    return _myNumChannels;
}

Time WMADecoder2::getDuration() const {
    return 0.0;
}

std::string WMADecoder2::getName() const {
    return _myURI;
}

void WMADecoder2::setSampleSink(asl::ISampleSink* mySampleSink) {
    AC_DEBUG << "setSampleSink";
    _mySampleSink = mySampleSink;
}

bool WMADecoder2::isSyncDecoder() const {
    return false;
}

void WMADecoder2::setTime(asl::Time myTime) {
    if (_myUseUserClock) {
        IWMReaderAdvanced * myAdvReader; 
        HRESULT hr = _myReader->QueryInterface(IID_IWMReaderAdvanced,(void **) &myAdvReader);
        checkForWMError(hr, string("QueryInterface(IWMReaderAdvanced) failed for ") + _myURI,
                PLUS_FILE_LINE);

        // Additional one second of buffering.
        hr = myAdvReader->DeliverTime(QWORD((myTime+1.0)*1000*1000*10));
        checkForWMError(hr, string("DeliverTime failed for ") + _myURI,
                PLUS_FILE_LINE);
    }
}

HRESULT STDMETHODCALLTYPE
WMADecoder2::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR * ppvObject) {
    if ((riid == IID_IWMReaderCallback) || (riid == IID_IUnknown)) {
        *ppvObject = static_cast<IWMReaderCallback *>(this);
        AddRef();
        return S_OK;
    }

    *ppvObject = NULL;
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE
WMADecoder2::AddRef() {
    return InterlockedIncrement(&_myReferenceCount);
}

ULONG STDMETHODCALLTYPE
WMADecoder2::Release() {
    if (InterlockedDecrement(&_myReferenceCount) == 0) {
        // This will not work with our smart pointers
        //delete this;
        return 0;
    }

    return _myReferenceCount;
}

HRESULT STDMETHODCALLTYPE WMADecoder2::OnSample(DWORD theOutputNumber,
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
            myAudioBuffer = _mySampleSink->createBuffer(numFrames);
            myAudioBuffer->convert(_myResampledSamples.begin(), SF_S16, 
                    _myNumChannels);
        } else {
            myAudioBuffer = _mySampleSink->createBuffer(numFrames);
            myAudioBuffer->convert(myBuffer, SF_S16, _myNumChannels);
        }
        _mySampleSink->queueSamples(myAudioBuffer);
        SetEvent(_mySampleEvent);
        AC_DEBUG << "OnSample, BufferLength: " << myBufferLength;
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE WMADecoder2::OnStatus(WMT_STATUS theStatus,
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

void WMADecoder2::open() {
    AC_DEBUG << "WMADecoder2::open (" << _myURI << ")";
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
    if (hr == NS_E_INVALID_REQUEST) {
        AC_WARNING << "WMADecoder2::open: Stream does not support user-provided clock.";
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

void WMADecoder2::close() {
    AC_DEBUG << "WMADecoder2::close() (" << _myURI << ")";
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
WMADecoder2::setupAudio() {
    AC_DEBUG << "WMADecoder2::setupAudio()";

    DWORD myOutputCount = 0;
    HRESULT hr = _myReader->GetOutputCount(&myOutputCount);
    checkForWMError(hr, "Could not get output count.", PLUS_FILE_LINE);

    _myAudioOutputId = -1;

    for (unsigned i = 0; i < myOutputCount; ++i) {
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
                _myResampleContext = audio_resample_init(_myNumChannels, _myNumChannels,    
                        Pump::get().getNativeSampleRate(), _mySampleRate);
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

void WMADecoder2::preroll(asl::Time thePosition) {
    AC_DEBUG << "WMADecoder2::preroll(" << (double)thePosition << ")";
    ResetEvent(_mySampleEvent);
    IWMReaderAdvanced2 * myAdvReader; 
    HRESULT hr = _myReader->QueryInterface(IID_IWMReaderAdvanced2,(void **) &myAdvReader);
    checkForWMError(hr, string("QueryInterface(IWMReaderAdvanced2) failed for ") + _myURI,
            PLUS_FILE_LINE);
    myAdvReader->Preroll(QWORD((double)thePosition*1000*1000*10), 0, 1);
}

void WMADecoder2::waitForSamples() {
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

}

