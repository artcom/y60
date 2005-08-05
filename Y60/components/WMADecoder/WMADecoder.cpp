//=============================================================================
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: WMADecoder.cpp,v $
//     $Author: christian $
//   $Revision: 1.1 $
//       $Date: 2005/04/08 18:38:36 $
//
//  WindowsMediaAudio (WMA) decoder class.
//
//=============================================================================

#include "WMADecoder.h"

#include <asl/Auto.h>
#include <asl/string_functions.h>
#include <asl/Logger.h>

#include <atlbase.h> // For CComPtr and CComQIPtr
#include <dsound.h>  // For the DSSPEAKER_5POINT1 value
#include <mmreg.h>   // For WAVEFORMATEXTENSIBLE (if you use it)

using namespace std;

#define DB(x) // x

namespace y60 {

    void
    checkForError(HRESULT hr, const std::string & theMessage, const std::string & theFileLine) {
        if (FAILED(hr)) {
            if (FACILITY_WINDOWS == HRESULT_FACILITY(hr)) {
                hr = HRESULT_CODE(hr);
            }
            std::string myMessage;
            char * szErrMsg;
            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
                NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&szErrMsg, 0, NULL) != 0)
            {
                myMessage = szErrMsg;
                LocalFree(szErrMsg);
            } else {
                char myErrorNumber[1024];
                sprintf(myErrorNumber, "Error # %#x", hr);
                myMessage = myErrorNumber;
            }
            throw WindowsMediaAudioException(theMessage + " [" + myMessage + "]", theFileLine);
        }
    }

    // TODO: This will not work with plugins. We need to use the sigleton manager
    static int ourWMADecoderCount = 0;

    //////////////////////////////////////////////////////////////////////////////////

    WMADecoder::WMADecoder(const std::string & theFilename, double theVolume, unsigned theMaxChannels) :
        BufferedSource(std::string("WMADecoder=" + theFilename)),    
        _myReferenceCount(0),
        _myReader(0),
        _myMaxChannels(theMaxChannels),
        _myEventResult(S_OK),
        _myAudioOutputId(-1)
    {
        if ((ourWMADecoderCount++) == 0) {
            HRESULT hr = CoInitialize(NULL);
            checkForError(hr, "CoInitialize failed", PLUS_FILE_LINE);
        }

        _myEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (!_myEvent) {
            throw WindowsMediaAudioException("Create Event failed", PLUS_FILE_LINE);
        }

        setVolume(theVolume);
        load(theFilename);
    }

    WMADecoder::~WMADecoder() {
        DB(cerr << "WMADecoder::~WMADecoder" << endl);

        if (_myEvent) {
            CloseHandle(_myEvent);
        }

        if (_myReader) {
            _myReader->Release();
            _myReader = NULL;
        }

        if ((--ourWMADecoderCount) == 0) {
            CoUninitialize();
        }

        if (_myReferenceCount != 0) {
            cerr << "### WARNING: There are " << _myReferenceCount << " references to WMADecoder during delete." << endl;
        }
    }

	double
    WMADecoder::getDuration() const {
        return 0.0;
    }

    void
    WMADecoder::stop() {
        DB(cerr << "WMADecoder::stop" << endl);
        HRESULT hr = _myReader->Close();

        checkForError(hr, "1+Could not stop Reader", PLUS_FILE_LINE);
        waitForEvent();
        checkForError(_myEventResult, "2+Could not stop Reader", PLUS_FILE_LINE);
    }

    void
    WMADecoder::pause() {
        DB(cerr << "WMADecoder::pause" << endl);
        BufferedSource::pause();

        HRESULT hr;
        if (isRunning()) {
            hr = _myReader->Resume();
        } else {
            hr = _myReader->Pause();
        }
    }

    bool
    WMADecoder::waitForEvent(unsigned theWaitTime) {
        DWORD myResult = WaitForSingleObject(_myEvent, theWaitTime);
        if (myResult == WAIT_OBJECT_0) {
            // The event handle was set to signaled state
            return true;
        } else if (myResult == WAIT_TIMEOUT) {
            //throw WindowsMediaAudioException("Time out waiting for the event.", PLUS_FILE_LINE);
            cerr << "### WARNING : WMADecoder timeout waiting for event" << endl;
            return false;
        } else if (myResult == WAIT_FAILED) {
            throw WindowsMediaAudioException("WaitForSingleObject failed.", PLUS_FILE_LINE);
        }
        return false;
    }

    void
    WMADecoder::load(const std::string & theUrl) {
        DB(cerr << "WMADecoder::load " << theUrl << endl);
        ResetEvent(_myEvent);
        HRESULT hr = WMCreateReader(NULL, WMT_RIGHT_PLAYBACK, &_myReader);
        checkForError(hr, "Could not create reader", PLUS_FILE_LINE);

        // Convert string to wstring
        size_t mySizeNeeded = ::mbstowcs(0, theUrl.c_str(), theUrl.length());
        std::wstring myWideUrl;
        myWideUrl.resize(mySizeNeeded);
        ::mbstowcs(&myWideUrl[0], theUrl.c_str(), theUrl.length());

        // Open file
        hr = _myReader->Open(myWideUrl.c_str(), this, 0);
        checkForError(hr, string("1+Could not open file: ") + theUrl, PLUS_FILE_LINE);
        waitForEvent();
        checkForError(_myEventResult, string("2+Could not open file: ") + theUrl, PLUS_FILE_LINE);
        if (_myMaxChannels > 2) {
            hr = setupMultiChannel();
            checkForError(hr, string("2+Could not setup Multichannel for file: ") + theUrl, PLUS_FILE_LINE);
        }
        setupAudio(theUrl);

        // Start playing
        hr = _myReader->Start(QWORD(0), 0, 1, NULL);
        checkForError(hr, "1+Could not start reader", PLUS_FILE_LINE);
        waitForEvent();
        checkForError(_myEventResult, "2+Could not start reader", PLUS_FILE_LINE);

        DB(cerr << "+++ Open succeeded +++" << endl);
		// everything super: insert module into the audio graph
		go();
    }

    void
    WMADecoder::setupAudio(const std::string & theUrl) {
        unsigned myAudioNumberOfChannels  = 2;
        unsigned myAudioSampleRate = 48000;
        unsigned myAudioBitsPerSample     = 16;

        DWORD myOutputCount = 0;
        HRESULT hr = _myReader->GetOutputCount(&myOutputCount);
        checkForError(hr, "Could not get output count.", PLUS_FILE_LINE);

        _myAudioOutputId = -1;

        for (unsigned i = 0; i < myOutputCount; ++i) {
            IWMOutputMediaProps * myOutputProperties = 0;
            HRESULT hr = _myReader->GetOutputProps(i, &myOutputProperties);
            checkForError(hr, string("Could not get output properties ") + asl::as_string(i) +
                " of " + asl::as_string(myOutputCount) +
                " from reader.", PLUS_FILE_LINE);

            // Get the media type structure
            DWORD myMediaTypeSize;
            hr = myOutputProperties->GetMediaType(NULL, &myMediaTypeSize);
            checkForError(hr, string("Could not mediatypesize of output ") +
                asl::as_string(i) + " from IWMOutputMediaProps", PLUS_FILE_LINE);
            WM_MEDIA_TYPE * myMediaType = (WM_MEDIA_TYPE *) new BYTE[myMediaTypeSize];
            hr = myOutputProperties->GetMediaType(myMediaType, &myMediaTypeSize);
            checkForError(hr, string("Could not mediatype of output ") +
                asl::as_string(i) + " from IWMOutputMediaProps", PLUS_FILE_LINE);

            GUID myMajorType = myMediaType->majortype;
            if (myMajorType == WMMEDIATYPE_Audio) {
                DB(cerr << "Output " << i << " is an audio stream" << endl;)
                _myAudioOutputId = i;
            } else if (myMajorType == WMMEDIATYPE_Script) {
                DB(cerr << "Output " << i << " is an script stream" << endl;)
            } else if (myMajorType == WMMEDIATYPE_FileTransfer) {
                DB(cerr << "Output " << i << " is an data stream" << endl;)
            } else if (myMajorType == WMMEDIATYPE_Image) {
                DB(cerr << "Output " << i << " is an image stream" << endl;)
            } else if (myMajorType == WMMEDIATYPE_Text) {
                DB(cerr << "Output " << i << " is an text stream" << endl;)
            }

            if (myMediaType->formattype == WMFORMAT_WaveFormatEx) {
                WAVEFORMATEX * myAudioInfo = (WAVEFORMATEX *)myMediaType->pbFormat;
                if (myAudioInfo->cbSize == 22) {
                    cout << "extension" << endl;
                }
                myAudioNumberOfChannels  = myAudioInfo->nChannels ;
                myAudioSampleRate = myAudioInfo->nSamplesPerSec;
                myAudioBitsPerSample    = myAudioInfo->wBitsPerSample;
            }

            if (myMediaType) {
                delete[] myMediaType;
            }

            if (myOutputProperties) {
                myOutputProperties->Release();
            }
        }

        if (_myAudioOutputId == -1) {
            throw WindowsMediaAudioException(std::string("No audio stream found"), PLUS_FILE_LINE);
        }
        if (myAudioBitsPerSample != 16) {
            throw WindowsMediaAudioException(std::string("Unsupported bits-per-sample:") + asl::as_string(myAudioBitsPerSample), PLUS_FILE_LINE);
        }

        AC_INFO << "  Number of channels: " << myAudioNumberOfChannels;
        AC_INFO << "  Samples per second: " << myAudioSampleRate;
        AC_INFO << "  Bits per sample:    " << myAudioBitsPerSample;

        setSampleBits(myAudioBitsPerSample);
        setup(myAudioNumberOfChannels, myAudioSampleRate);
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

    HRESULT STDMETHODCALLTYPE
    WMADecoder::OnSample(DWORD theOutputNumber,
                             QWORD theSampleTime,
                             QWORD theSampleDuration,
                             DWORD theFlags,
                             INSSBuffer __RPC_FAR * theSample,
                             void __RPC_FAR * theContext)
    {
        if (theOutputNumber == _myAudioOutputId) {
            asl::AutoLocker<BufferedSource> myLocker(*this);
            BYTE * myBuffer;
            DWORD myBufferLength;
            HRESULT hr = theSample->GetBufferAndLength(&myBuffer, &myBufferLength);
            checkForError(hr, "Could not get buffer from sample", PLUS_FILE_LINE);
            addBuffer(theSampleTime / 10000000.0, myBuffer, myBufferLength);
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    WMADecoder::OnStatus(WMT_STATUS theStatus,
                             HRESULT hr,
                             WMT_ATTR_DATATYPE theType,
                             BYTE __RPC_FAR * theValue,
                             void __RPC_FAR * theContext)
    {
        asl::AutoLocker<BufferedSource> myLocker(*this);
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
                setRunning(true);
                break;
            case WMT_STOPPED:
                DB(cerr << ">>> Stopped. " << (void*)this << endl;)
                _myEventResult = hr;
                SetEvent(_myEvent);
                BufferedSource::stop();
                break;
            case WMT_CLOSED:
                DB(cerr << ">>> Closed the file. " << (void*)this << endl;)
                _myEventResult = hr;
                SetEvent(_myEvent);
                BufferedSource::stop();
                break;
            case WMT_EOF:
                DB(cerr << ">>> End of file." << endl;)
                _myEventResult = hr;
                SetEvent(_myEvent);
                BufferedSource::stop();
                break;
            default:
                //DB(cerr << ">>> OnStatus " << theStatus << endl);
                break;
        }

        return S_OK;
    }

    // Does not work for string attributes
    template <class T>
    void
    WMADecoder::getStreamAttribute(IWMHeaderInfo * theHeaderInfo, const std::string & theName, T & theResult) {
        // Convert string to wstring
        size_t mySizeNeeded = ::mbstowcs(0, theName.c_str(), theName.length());
        std::wstring myWideName;
        myWideName.resize(mySizeNeeded);
        ::mbstowcs(&myWideName[0], theName.c_str(), theName.length());

        WMT_ATTR_DATATYPE myType;
        WORD myValueLength;
        WORD myStreamNumber = 0;
        HRESULT hr = theHeaderInfo->GetAttributeByName(&myStreamNumber, myWideName.c_str(), &myType, NULL, &myValueLength);
        checkForError(hr, "Could not get Attribute size from video stream", PLUS_FILE_LINE);
        // Check types
        const std::type_info * sourceType = NULL;
        switch (myType) {
            case WMT_TYPE_DWORD:
                sourceType = &typeid(DWORD);
                break;
            case WMT_TYPE_QWORD:
                sourceType = &typeid(QWORD);
                break;
            case WMT_TYPE_STRING:
                // XXX: Implement conversion
                throw WindowsMediaAudioException(string("Stream-Attribute ") + theName + " has unimplemented WMT_TYPE_STRING type", PLUS_FILE_LINE);
                break;
            case WMT_TYPE_BINARY:
                // XXX: Implement conversion
                throw WindowsMediaAudioException(string("Stream-Attribute ") + theName + " has unimplemented WMT_TYPE_BINARY type", PLUS_FILE_LINE);
                break;
            case WMT_TYPE_BOOL:
                sourceType = &typeid(bool);
                break;
            case WMT_TYPE_WORD:
                sourceType = &typeid(WORD);
                break;
            case WMT_TYPE_GUID:
                // XXX: Implement conversion
                throw WindowsMediaAudioException(string("Stream-Attribute ") + theName + " has unimplemented WMT_TYPE_GUID type", PLUS_FILE_LINE);
                break;
        }
        if (sourceType == 0) {
            throw WindowsMediaAudioException(string("Stream-Attribute ") + theName + " has invalid type", PLUS_FILE_LINE);
        }
        if (*sourceType != typeid(T)) {
            throw WindowsMediaAudioException(string("Stream-Attribute ") + theName + " has wrong type " + sourceType->name() + ". Should be: " + typeid(T).name(), PLUS_FILE_LINE);
        }
        hr = theHeaderInfo->GetAttributeByName(&myStreamNumber, myWideName.c_str(), &myType, (BYTE*)&theResult, &myValueLength);
        if (hr == ASF_E_NOTFOUND) {
            throw WindowsMediaAudioException(string("Stream-Attribute ") + theName + " was not found", PLUS_FILE_LINE);
        } else {
            checkForError(hr, "Could not get Attribute from video stream", PLUS_FILE_LINE);
        }
    }

    HRESULT WMADecoder::setupMultiChannel() {
        //Set up multichannel playback
        BOOL fEnableDiscreteOutput = TRUE;
        //DWORD dwSpeakerConfig = DSSPEAKER_5POINT1;
        DWORD dwSpeakerConfig = DSSPEAKER_DIRECTOUT;
        HRESULT hr = E_FAIL;

        CComQIPtr<IWMReaderAdvanced2, &IID_IWMReaderAdvanced2>
            pReaderAdvanced2(_myReader);
        if(! pReaderAdvanced2)
            return E_FAIL;
        //Make the required settings on the Reader Object
        hr = pReaderAdvanced2->SetOutputSetting(0,
            g_wszEnableDiscreteOutput,
            WMT_TYPE_BOOL,
            (BYTE *)&fEnableDiscreteOutput,
            sizeof( BOOL ) );
        if(FAILED(hr)) return hr;

        hr = pReaderAdvanced2->SetOutputSetting(0,
            g_wszSpeakerConfig,
            WMT_TYPE_DWORD,
            (BYTE *)&dwSpeakerConfig,
            sizeof( DWORD ) );
        if(FAILED(hr)) return hr;

        // Dynamic range control can also be set using SetOutputSetting,
        // although we don't show that here.

        // Get the various formats supported by the audio output.
        // In this example, to keep things simple, we only handle
        // audio-only files with a single stream. In other words, we
        // assume that there is one audio output and that its number is zero.
        DWORD dwAudioOutput = 0;
        DWORD formats = 0;
        hr = _myReader->GetOutputFormatCount(dwAudioOutput, &formats);
        if(FAILED(hr)) return hr;

        // Multichannel formats, if available, are returned first
        for(int j = 0; j < formats;j++)
        {
            CComPtr<IWMOutputMediaProps> pProps;
            hr = _myReader->GetOutputFormat(dwAudioOutput, j, &pProps);
            if(FAILED(hr)) break;

            WM_MEDIA_TYPE* pNativeType = NULL;
            DWORD cbFormat = 0;
            hr = pProps->GetMediaType( NULL, &cbFormat );
            if(FAILED(hr)) break;

            pNativeType = (WM_MEDIA_TYPE *)new BYTE[ cbFormat ];
            if( NULL == pNativeType )
            {
                printf( "Not enough core\n" );
                hr = E_OUTOFMEMORY;
            }

            hr = pProps->GetMediaType( pNativeType, &cbFormat );
            if( hr != S_OK )
            {
                printf( "Failed getting the media type\n");
                return hr;
            }
            //  This works for WAVEFORMATEXTENSIBLE formats as long
            //  as we are only looking at the WAVEFORMATEX members.
            WAVEFORMATEX* pWFX = (WAVEFORMATEX*) pNativeType->pbFormat;
            if(pWFX->nChannels == 6)
            {
                // We have found a six-channel output supported for
                // this file. If we were going to examine all the contents
                // of the structure, we would need to cast
                // pNativeType->pbFormat to WAVEFORMATEXTENSIBLE. In this
                // example, we just set the first multichannel type
                // we find. The format block has been correctly
                // allocated, so we just pass it to SetMediaType.
                hr = pProps->SetMediaType(pNativeType);
                if( hr != S_OK )
                {
                    printf( "Failed to set output props\n");
                    return hr;
                }

                hr = _myReader->SetOutputProps(0, pProps);
                if( hr != S_OK )
                {
                    printf( "Failed to set output props\n");
                    return hr;
                }
                delete pNativeType;
                break;

            } // end if

            delete pNativeType;

        } // end for
        return S_OK;
    }
}
