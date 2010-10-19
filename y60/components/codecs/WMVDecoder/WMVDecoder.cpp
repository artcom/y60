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
//
//    $RCSfile: WMVDecoder.cpp,v $
//     $Author: thomas $
//   $Revision: 1.11 $
//       $Date: 2005/04/19 09:28:22 $
//
//  WindowsMediaVideo (WMV) decoder class.
//
//=============================================================================

#include "WMVDecoder.h"

#include <y60/video/Movie.h>

#include <asl/base/Auto.h>
#include <asl/audio/Pump.h>
#include <asl/base/Logger.h>
#include <asl/base/os_functions.h>

#include <typeinfo>
#include <atlbase.h> //for CComPtr and CComQIPtr
#include <dsound.h> //For the DSSPEAKER_5POINT1 value
#include <mmreg.h> //For WAVEFORMATEXTENSIBLE (if you use it)


using namespace std;
using namespace asl;
using namespace y60;

#define DB(x) //x
#define DB2(x) //x

extern "C"
EXPORT asl::PlugInBase * WMVDecoder_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::WMVDecoder(myDLHandle);
}

namespace y60 {

    static const unsigned FRAME_CACHE_SIZE = 32; // 256
    asl::Block WMVDecoder::_myResampledSamples(AVCODEC_MAX_AUDIO_FRAME_SIZE);

    bool
    checkForError(HRESULT hr, const std::string & theMessage, const std::string & theFileLine) {
		std::string myWindowsMessage;
		if (hResultIsOk(hr, myWindowsMessage) == false) {
            AC_ERROR << "Error in WMVReader " << theMessage << theFileLine;
            return false;
		}
		return true;
    }

    /*
     * VideoFrame with timestamp
     */
    WMVDecoder::VideoFrame::VideoFrame(double theTimestamp, INSSBuffer __RPC_FAR & theBuffer) {
        _myTimestamp = theTimestamp;
        _myBuffer = &theBuffer;
        //AC_DEBUG << "AddRef " << (void*)_myBuffer;
        _myBuffer->AddRef();
    }

    WMVDecoder::VideoFrame::~VideoFrame() {
        //AC_DEBUG << "Release " << (void*)_myBuffer;
        _myBuffer->Release();
        _myBuffer = 0;
    }

    /*
     * WMVDecoder plug-in
     */
    WMVDecoder::WMVDecoder(asl::DLHandle theDLHandle) :
        AsyncDecoder(), PlugInBase(theDLHandle),
        _myFrameCacheSize(FRAME_CACHE_SIZE),
        _myVideoOutputId(-1),
        _myAudioOutputId(-1),
        _myReferenceCount(0),
        _myReader(NULL),
        _myEventResult(S_OK),
        _myCurrentPlaySpeed(1.0),
        _myFrameRate(0.0),
        _myFirstFrameDelivered(false),
        _myLastVideoTimeStamp(0.0),
        _myLastAudioTimeStamp(0.0),
        _myResampleContext(0),
        _myReadEOF(false),
        _hasShutDown(false)
    {
        AC_DEBUG << "WMVDecoder::WMVDecoder " << (void*)this;
        CoInitialize(0);

        _myEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (!_myEvent) {
            throw WindowsMediaException("CreateEvent failed", PLUS_FILE_LINE);
        }
    }

    void WMVDecoder::shutdown() {
        if (!_hasShutDown) {
            DB(AC_DEBUG << "WMVDecoder::shutdown()");
            closeMovie();
            releaseBuffers();

            if (_myEvent) {
                CloseHandle(_myEvent);
                _myEvent = 0;
            }

            if (_myReader) {
                _myReader->Release();
                _myReader = NULL;
            }

            if (_myResampleContext) {
                audio_resample_close(_myResampleContext);
                _myResampleContext = 0;
            }

            if (_myReferenceCount != 0) {
                AC_WARNING << "Still have " << _myReferenceCount << " references to WMVDecoder";
            }
            CoUninitialize();
        }
        _hasShutDown = true;
    }
    WMVDecoder::~WMVDecoder() {
        AC_DEBUG << "WMVDecoder::~WMVDecoder " << (void*)this;
        shutdown();
    }

    asl::Ptr<MovieDecoderBase> WMVDecoder::instance() const {
        return asl::Ptr<MovieDecoderBase>(new WMVDecoder(getDLHandle()));
    }

    std::string
    WMVDecoder::canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream) {
        const std::string myExtension = asl::toLowerCase(asl::getExtension(theUrl));
        if (myExtension == "wmv") {
            return MIME_TYPE_WMV;
        } else if (myExtension == "avi") {
            return MIME_TYPE_AVI;
        } else {
            return "";
        }
    }

    void
    WMVDecoder::resetEvent() {
        ResetEvent(_myEvent);
    }

    void
    WMVDecoder::releaseBuffers() {
        AC_DEBUG << "WMVDecoder::releaseBuffers size=" << _myFrameCache.size();
#if 1
        _myFrameCache.clear();
#else
        while (_myFrameCache.size() > 0) {
            _myFrameCache.pop_back();
        }
#endif
    }

    void
    WMVDecoder::load(const std::string & theUrl) {
        AC_INFO << "WMVDecoder::load " << theUrl;

        _myFirstFrameDelivered = false;
        _myLastVideoTimeStamp = _myLastAudioTimeStamp = 0.0;

        resetEvent();
        HRESULT hr = WMCreateReader(NULL, WMT_RIGHT_PLAYBACK, &_myReader);
        if (!checkForError(hr, "WMCreateReader failed", PLUS_FILE_LINE)) {
            throw WindowsMediaException("WMCreateReader failed", PLUS_FILE_LINE);
        }
		//checkNetworkConfig(); // this function must be cleaned, release handles ,etc, VS

        // Convert string to wstring
        size_t mySizeNeeded = ::mbstowcs(0, theUrl.c_str(), theUrl.length());
        std::wstring myWideUrl;
        myWideUrl.resize(mySizeNeeded);
        ::mbstowcs(&myWideUrl[0], theUrl.c_str(), theUrl.length());

        hr = _myReader->Open(myWideUrl.c_str(), this, 0);
        if (!checkForError(hr, string("1+Could not open file: ") + theUrl, PLUS_FILE_LINE)) {
            _myReader->Release();
            _myReader = NULL;
            throw WindowsMediaException(string("1+Could not open file: ") + theUrl, PLUS_FILE_LINE);
        }
        waitForEvent();
        if (!checkForError(_myEventResult, string("2+Could not open file: ") + theUrl, PLUS_FILE_LINE)) {
            _myReader->Release();
            _myReader = NULL;
            throw WindowsMediaException(string("2+Could not open file: ") + theUrl, PLUS_FILE_LINE);
        }
#if 0
        hr = setupMultiChannel();
        checkForError(hr, string("Could not setup Multichannel for file: ") + theUrl, PLUS_FILE_LINE);
#endif
        setupVideoAndAudio(theUrl);
        AC_DEBUG << "+++ Open succeeded url=" << theUrl;
    }

    double
    WMVDecoder::readFrame(double theTime, unsigned /*theFrame*/, RasterVector theTargetRaster) {
        AC_DEBUG << "WMVDecoder::readFrame: " << theTime;
        // EOF and framecache empty?
        if (_myReadEOF && _myFrameCache.size() <= 1) {
            AC_DEBUG << "EoF and FrameCache empty";
            setEOF(true);
            if (_myAudioSink) {
                _myAudioSink->stop();
            }
            _myReadEOF = false;
        }
        if (getPlayMode() != PLAY_MODE_PLAY) {
            return theTime;
        }

        // Restart if playspeed has changed
        if (getMovie()->get<PlaySpeedTag>() != _myCurrentPlaySpeed) {
            double myLastDecodedTimestamp = _myLastVideoTimeStamp;
            stopMovie();
            startMovie(float(myLastDecodedTimestamp));
        }

        asl::AutoLocker<ThreadLock> myLocker(_myLock); // protect shared vars (_myFrameCache, ...)
		getMovie()->set<CacheSizeTag>(_myFrameCache.size()); // purely informational, sets value in DOM

        if (_myFirstFrameDelivered == false) {
            // Clear raster
            static unsigned char myValue = 0;
            memset(theTargetRaster[0]->pixels().begin(), myValue++, theTargetRaster[0]->pixels().size());
        }

        if (_myFrameCache.empty()) {
            AC_TRACE << "FrameCache is empty";
            return theTime;
        }

        if (theTime > _myLastVideoTimeStamp) {
            AC_TRACE << "WMVDecoder::readFrame FrameCache underrun, time=" << theTime << " last in cache " << _myLastVideoTimeStamp;
            theTime = _myLastVideoTimeStamp;
        }

        // Find 'closest' frame in VideoFrameCache
        double myMinTimeDiff = 1000000.0;
        VideoFramePtr myBestFrame;
        unsigned myCachePosition = 0;
        unsigned myFoundCachePosition = 0;

        for (VideoFrameCache::const_iterator it = _myFrameCache.begin(); it != _myFrameCache.end(); ++it) {
            VideoFramePtr myVideoFrame(*it);
            //double myTimeDiff = fabs(theTime - myVideoFrame->getTimestamp());
            double myTimeDiff = theTime - myVideoFrame->getTimestamp();
            AC_TRACE << "timediff : " << myTimeDiff;
            if (myTimeDiff >= 0.0 && myTimeDiff < myMinTimeDiff) {
                myMinTimeDiff = myTimeDiff;
                myBestFrame = myVideoFrame;
                myFoundCachePosition = myCachePosition;
                AC_TRACE << "WMVDecoder::readFrame Found frame=" << myVideoFrame->getTimestamp() << " for time=" << theTime;
            }
            ++myCachePosition;
        }

        if (myBestFrame == 0) {
            AC_TRACE << "No good frame found for time=" << theTime;
            return theTime;
        }


        // drop all frames older than the one we just found
        for (;;) {
            VideoFramePtr myVideoFrame(_myFrameCache.front());
            if (myVideoFrame == myBestFrame) {
                break;
            }
            _myFrameCache.pop_front();
        }

        BYTE * myBuffer;
        DWORD myBufferLength;
        HRESULT hr = myBestFrame->getBuffer()->GetBufferAndLength(&myBuffer, &myBufferLength);
        if (checkForError(hr, "GetBufferAndLength failed", PLUS_FILE_LINE)) {

            // Copy data to raster
            theTargetRaster[0]->resize(getFrameWidth(), getFrameHeight());
            if (myBufferLength == theTargetRaster[0]->pixels().size()) {
                memcpy(theTargetRaster[0]->pixels().begin(), myBuffer, myBufferLength);
                _myFirstFrameDelivered = true;
            } else {
                AC_WARNING << "TargetRaster has wrong size. buffer=" << myBufferLength << " raster=" << theTargetRaster[0]->pixels().size();
            }
        } else {
            AC_ERROR << "GetBufferAndLength failed";
            memset(theTargetRaster[0]->pixels().begin(), 0xff, theTargetRaster[0]->pixels().size());
        }

        //return theTime;
        return myBestFrame->getTimestamp();
    }

    void
    WMVDecoder::resumeMovie(double theStartTime, bool theResumeAudioFlag) {
        AC_DEBUG << "WMVDecoder::resumeMovie " << (void*)this << " time=" << theStartTime;
        if (_myReader) {
            resetEvent();
            asl::AutoLocker<ThreadLock> myLocker(_myLock);
            HRESULT hr = _myReader->Resume();
            checkForError(hr, "Could not resume WMVDecoder", PLUS_FILE_LINE);
        }
        AsyncDecoder::resumeMovie(theStartTime, !_myCachingFlag);
    }

    void
    WMVDecoder::startMovie(double theStartTime, bool theStartAudioFlag) {
        AC_DEBUG << "WMVDecoder::startMovie()";
        if (_myReader) {
            resetEvent();
            _myCurrentPlaySpeed = getMovie()->get<PlaySpeedTag>();
            HRESULT hr = _myReader->Start(QWORD(theStartTime * 10000000.0), 0, float(_myCurrentPlaySpeed), NULL);
            checkForError(hr, "Could not start WMVDecoder", PLUS_FILE_LINE);
            waitForEvent();
            checkForError(_myEventResult, "Starting playback failed.", PLUS_FILE_LINE);
        }
        AsyncDecoder::startMovie(theStartTime, false);
        if (_myReader) {
            asl::AutoLocker<ThreadLock> myLocker(_myLock);
            if (_myAudioSink) {
                // flush AudioBuffer
                _myCachingFlag = true;
                _myAudioSink->stop(!_myCachingFlag);
            } else {
                _myCachingFlag = false;
            }
        }
    }

    void
    WMVDecoder::stopMovie(bool theStopAudioFlag) {
        AC_DEBUG << "WMVDecoder::stopMovie()";
        if (_myReader) {
            resetEvent();
            HRESULT hr = _myReader->Stop();
            checkForError(hr, "Could not stop windows media reader", PLUS_FILE_LINE);
            waitForEvent();
            checkForError(_myEventResult, "Stopping playback failed.", PLUS_FILE_LINE);
            _myLastVideoTimeStamp = 0;
        }

        releaseBuffers();
        _myFirstFrameDelivered = false;

        AsyncDecoder::stopMovie();
    }

    void
    WMVDecoder::pauseMovie(bool thePauseAudioFlag) {
        AC_DEBUG << "WMVDecoder::pauseMovie()";
        asl::AutoLocker<ThreadLock> myLocker(_myLock);
        if (_myReader) {
            resetEvent();
            HRESULT hr = _myReader->Pause();
            checkForError(hr, "Could not pause windows media reader", PLUS_FILE_LINE);
        }
        AsyncDecoder::pauseMovie(!_myCachingFlag);
    }

    void
    WMVDecoder::closeMovie() {
        AC_DEBUG << "WMVDecoder::closeMovie";
        if (_myReader) {
            /*HRESULT hr =*/ _myReader->Close();
            //checkForError(hr, "Could not close Reader.", PLUS_FILE_LINE);
            waitForEvent();
            //checkForError(_myEventResult, "Stopping playback failed.", PLUS_FILE_LINE);
        }
        AsyncDecoder::closeMovie();
    }

    bool
    WMVDecoder::waitForEvent(unsigned theWaitTime) {
        DWORD myResult = WaitForSingleObject(_myEvent, theWaitTime);
        if (myResult == WAIT_OBJECT_0) {
            // The event handle was set to signaled state
            return true;
        }
        else if (myResult == WAIT_TIMEOUT) {
            //throw WindowsMediaException("Time out waiting for the event.", PLUS_FILE_LINE);
            AC_ERROR << "WMVDecoder timeout waiting for event";
        }
        else if (myResult == WAIT_FAILED) {
            throw WindowsMediaException("WaitForSingleObject failed.", PLUS_FILE_LINE);
        }
        return false;
    }

    std::wstring getWStringFromString(const std::string & theString) {
        std::size_t mySizeNeeded = ::mbstowcs(0, theString.c_str(), theString.length());
        std::wstring myWideString;
        myWideString.resize(mySizeNeeded);
        ::mbstowcs(&myWideString[0], theString.c_str(), theString.length());
        return myWideString;
    }

    bool WMVDecoder::checkNetworkConfig() {
        // Get buffering time
 		IWMReaderNetworkConfig * myNetworkConfig;
 		HRESULT hr = _myReader->QueryInterface(IID_IWMReaderNetworkConfig, (VOID**)&myNetworkConfig);
 		if (FAILED(hr)) {
 		    return false;
 		}

 		// BufferingTime
 		QWORD myBufferingTime;
 		if (SUCCEEDED(myNetworkConfig->GetBufferingTime(&myBufferingTime))) {
 		    AC_DEBUG << "BufferingTime=" << (myBufferingTime / 10000000.0) << "s";
 		}
        myNetworkConfig->SetBufferingTime(2 * 10000000);

 		// ConnectionBandwidth
 		DWORD myBandwidth;
 		if (SUCCEEDED(myNetworkConfig->GetConnectionBandwidth(&myBandwidth))) {
 		    AC_DEBUG << "ConnectionBandwidth=" << myBandwidth;
 		}

 		// LoggingUrl
 		DWORD myUrlCount;
 		if (SUCCEEDED(myNetworkConfig->GetLoggingUrlCount(&myUrlCount))) {
            LPWSTR myUrl = 0;
            DWORD myUrlSize = 0;
            for (DWORD myIndex = 0; myIndex < myUrlCount; ++myIndex) {
                if (FAILED(myNetworkConfig->GetLoggingUrl(myIndex, 0, &myUrlSize))) {
                    break;
                }

                myUrl = new wchar_t[myUrlSize];
                HRESULT hr = myNetworkConfig->GetLoggingUrl(myIndex, myUrl, &myUrlSize);
                if (SUCCEEDED(hr)) {
                    AC_DEBUG << myIndex << "LoggingUrl=" << myUrl;
                }
                delete[] myUrl;
                myUrl = 0;
                if (FAILED(hr)) {
                    break;
                }
            }
        }

        setupNetworkConfig(myNetworkConfig, "http");
        setupNetworkConfig(myNetworkConfig, "mms");
        return true;
    }

    const std::string WMVDecoder::getProxyForProtocol(IWMReaderNetworkConfig * theNetworkConfig,
                                                      const std::wstring & theWideProtocol) {
        WMT_PROXY_SETTINGS myProxySettings;
        if (SUCCEEDED(theNetworkConfig->GetProxySettings(theWideProtocol.c_str(), &myProxySettings))) {
            switch (myProxySettings) {
            case WMT_PROXY_SETTING_NONE:
                return "NONE";
            case WMT_PROXY_SETTING_MANUAL:
                return "MANUAL";
            case WMT_PROXY_SETTING_AUTO:
                return "AUTO";
            case WMT_PROXY_SETTING_BROWSER:
                return "BROWSER";
            case WMT_PROXY_SETTING_MAX:
                return "MAX";
            }
        }
        return "NONE";
    }

    void WMVDecoder::setupNetworkConfig(IWMReaderNetworkConfig * theNetworkConfig, const std::string & theProtocol) {
        // ProxySettings
        std::wstring myWideProtocol = getWStringFromString("http");
        AC_DEBUG << "ProxySettings/" << theProtocol << "=" << getProxyForProtocol(theNetworkConfig, myWideProtocol);
        //theNetworkConfig->SetProxySettings(myWideProtocol.c_str(), WMT_PROXY_SETTING_NONE);

        // Bypass (used only when ProxySettings == MANUAL)
        BOOL myBypassForLocal;
        if (SUCCEEDED(theNetworkConfig->GetProxyBypassForLocal(myWideProtocol.c_str(), &myBypassForLocal))) {
            AC_DEBUG << "BypassForLocal/" << theProtocol << "=" << myBypassForLocal;
        }
    }

    void WMVDecoder::setupVideoAndAudio(const std::string & theUrl) {
        Movie * myMovie = getMovie();
        _myFrameRate = 25.0;
        unsigned myVideoWidth  = 0;
        unsigned myVideoHeight = 0;

        unsigned myAudioNumberOfChannels = 2;
        unsigned myAudioSampleRate       = 44100;
        unsigned myAudioBitsPerSample    = 16;

        DWORD myOutputCount = 0;
        if (!_myReader) {
            AC_ERROR << "setupVideoAndAudio called with NULL reader";
            return;
        }
        HRESULT hr = _myReader->GetOutputCount(&myOutputCount);
        checkForError(hr, "Could not get output count.", PLUS_FILE_LINE);

        _myVideoOutputId = -1;
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
            if (myMajorType == WMMEDIATYPE_Video) {
                AC_DEBUG << "Output " << i << " is a video stream";
                _myVideoOutputId = i;
            } else if (myMajorType == WMMEDIATYPE_Audio) {
                AC_DEBUG << "Output " << i << " is an audio stream";
                if (getAudioFlag()) {
                    _myAudioOutputId = i;
                } else {
                    AC_DEBUG<<"video has audio but it's disabled";
                }
            } else if (myMajorType == WMMEDIATYPE_Script) {
                AC_DEBUG << "Output " << i << " is an script stream";
            } else if (myMajorType == WMMEDIATYPE_FileTransfer) {
                AC_DEBUG << "Output " << i << " is an data stream";
            } else if (myMajorType == WMMEDIATYPE_Image) {
                AC_DEBUG << "Output " << i << " is an image stream";
            } else if (myMajorType == WMMEDIATYPE_Text) {
                AC_DEBUG << "Output " << i << " is an text stream";
            }

            if (myMediaType->formattype == WMFORMAT_VideoInfo) {
                // Setup frame size
                WMVIDEOINFOHEADER * myVideoHeader = (WMVIDEOINFOHEADER *)myMediaType->pbFormat;
                BITMAPINFOHEADER myBmiHeader = myVideoHeader->bmiHeader;

                myVideoWidth  = myBmiHeader.biWidth;
                myVideoHeight = myBmiHeader.biHeight;
                if (myVideoHeader->AvgTimePerFrame > 0) {
                    _myFrameRate = 1.0f / (myVideoHeader->AvgTimePerFrame / 10000000.0f);
                }

                // Setup pixel format
                GUID mySubType = myMediaType->subtype;
                if (mySubType == WMMEDIASUBTYPE_RGB24) {
                    myMovie->createRaster(myVideoWidth, myVideoHeight, 1, y60::BGR);
                } else if (mySubType == WMMEDIASUBTYPE_RGB32) {
                    myMovie->createRaster(myVideoWidth, myVideoHeight, 1, y60::BGRA);
                } else {
                    throw WindowsMediaException("Unsupported pixel format", PLUS_FILE_LINE);
                }
                myMovie->getRasterPtr()->clear();
            } else if (myMediaType->formattype == WMFORMAT_WaveFormatEx) {
                WAVEFORMATEX * myAudioInfo = (WAVEFORMATEX *)myMediaType->pbFormat;

                _myAudioNumberOfChannels = myAudioInfo->nChannels ;
                myAudioSampleRate       = myAudioInfo->nSamplesPerSec;
                myAudioBitsPerSample    = myAudioInfo->wBitsPerSample;
            }

            if (myMediaType) {
                delete[] myMediaType;
            }

            if (myOutputProperties) {
                myOutputProperties->Release();
            }
        }
        if (_myVideoOutputId < 0) {
            throw WindowsMediaException("No video stream found", PLUS_FILE_LINE);
        }

        IWMHeaderInfo * myHeaderInfo = NULL;
        hr = _myReader->QueryInterface(IID_IWMHeaderInfo, (void**)&myHeaderInfo);
        checkForError(hr, "Could not query for interface IWMHeaderInfo", PLUS_FILE_LINE);

        QWORD myDuration = 0;
        getStreamAttribute(myHeaderInfo, "Duration", myDuration);
		myHeaderInfo->Release();

        // Setup video frame count and frame rate/1
        if (myDuration > 0) {
            myMovie->set<FrameCountTag>(unsigned((myDuration / 10000000.f) * _myFrameRate));
        } else {
            // For streaming media
            myMovie->set<FrameCountTag>(UINT_MAX);
        }

        myMovie->set<FrameRateTag>(_myFrameRate);

        // Setup video size and image matrix
        //float myXResize = float(myVideoWidth) / asl::nextPowerOfTwo(myVideoWidth);
        //float myYResize = float(myVideoHeight) / asl::nextPowerOfTwo(myVideoHeight);

        asl::Matrix4f myMatrix;
        myMatrix.assign(1,0,0,0, 0,-1,0,0, 0,0,1,0, 0,1,0,1);
        myMovie->set<ImageMatrixTag>(myMatrix);

        AC_DEBUG << "Video: frame=" << myVideoWidth << "x" << myVideoHeight << " pixelFormat=" << asl::getStringFromEnum(myMovie->getRasterEncoding(), y60::PixelEncodingString);
        AC_DEBUG << "       fps=" << _myFrameRate << " duration=" << myDuration / 10000000.f << " s" << " frameCount=" << getFrameCount();

        // Cleanup audio buffer
        if (_myAudioSink) {
            _myAudioSink->stop();
        }

        // Setup audio controller
        if (_myAudioOutputId >= 0) {
            AC_DEBUG << "Audio: numChannels=" << myAudioNumberOfChannels
                     << " sampleRate=" << myAudioSampleRate
                     << " bitPerSample=" << myAudioBitsPerSample
                     << " native sampleRate=" <<Pump::get().getNativeSampleRate();

            _myAudioSink = Pump::get().createSampleSink(theUrl);

            if (myAudioSampleRate != Pump::get().getNativeSampleRate())
            {
                _myResampleContext = audio_resample_init(myAudioNumberOfChannels,
                        myAudioNumberOfChannels, Pump::get().getNativeSampleRate(),
                        myAudioSampleRate);
            }
            AC_INFO << "WMVDecoder::setupAudio() done. resampling "
                    << (_myResampleContext != 0);

        } else if (getAudioFlag()){
            AC_INFO << "Movie '" << theUrl << "' does not contain audio.";
        }

        // Set AV delay for WMV
        myMovie->set<AVDelayTag>(-0.35);

    }

    HRESULT STDMETHODCALLTYPE
    WMVDecoder::QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR * ppvObject) {
        if ((riid == IID_IWMReaderCallback) || (riid == IID_IUnknown)) {
            *ppvObject = static_cast<IWMReaderCallback *>(this);
            AddRef();
            return S_OK;
        }

        *ppvObject = NULL;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE
    WMVDecoder::AddRef() {
        //AC_DEBUG << "AddRef pre-inc-count=" << _myReferenceCount << " " << (void*)this;
        return InterlockedIncrement(&_myReferenceCount);
    }

    ULONG STDMETHODCALLTYPE
    WMVDecoder::Release() {
        //AC_DEBUG << "Release pre-dec-count=" << _myReferenceCount << " " << (void*)this;
        if (InterlockedDecrement(&_myReferenceCount) == 0) {
            // This will not work with our smart pointers
            //delete this;
            return 0;
        }

        return _myReferenceCount;
    }

    HRESULT STDMETHODCALLTYPE
    WMVDecoder::OnSample(DWORD theOutputNumber,
                         QWORD theSampleTime,
                         QWORD theSampleDuration,
                         DWORD theFlags,
                         INSSBuffer __RPC_FAR * theSample,
                         void __RPC_FAR * theContext)
    {
        double myTimeStamp = theSampleTime / 10000000.0;

        if (theOutputNumber == static_cast<DWORD>(_myVideoOutputId)) {
            //AC_TRACE << "VideoSample arrived: " << myTimeStamp << "s";
            asl::AutoLocker<ThreadLock> myLocker(_myLock);

            if (_myLastVideoTimeStamp && (myTimeStamp - _myLastVideoTimeStamp) > 1.0) {
                AC_TRACE << "Last VideoSample is " << (myTimeStamp - _myLastVideoTimeStamp) << "s old.";
            }
            _myLastVideoTimeStamp = myTimeStamp;

            // Push to VideoFrameCache, limit size when not caching
            _myFrameCache.push_back(VideoFramePtr(new VideoFrame(myTimeStamp, *theSample)));
            if (_myFrameCache.size() > _myFrameCacheSize-1) {
                AC_TRACE << "FrameCache overrun, dropping oldest frame cache size: " << _myFrameCache.size();
                _myFrameCache.pop_front();
            }

            // Start audio when frameCache is full enough
            double myStartTime = (_myFrameCacheSize/2) / _myFrameRate;
            if (_myCachingFlag &&
                ((_myFrameCache.size() >= _myFrameCacheSize/2) || (_myLastAudioTimeStamp >= myStartTime))) {
                AC_DEBUG << "Starting A/V playback, FrameCache size=" << _myFrameCache.size() << " max cache size=" << _myFrameCacheSize;
                AC_DEBUG << "                       LastAudioTimeStamp=" << _myLastAudioTimeStamp << " StartTime=" << myStartTime;
                _myCachingFlag = false;
                if (_myAudioSink) {
                    _myAudioSink->play();
                }
            }

            // Signal, new buffer is ready
            //SetEvent(_myEvent);
        } else if (_myAudioSink && theOutputNumber == static_cast<DWORD>(_myAudioOutputId)) {
            //AC_TRACE << ">>> AudioSample arrived: " << myTimeStamp << "s";

            BYTE * myBuffer;
            DWORD myBufferLength;
            HRESULT hr = theSample->GetBufferAndLength(&myBuffer, &myBufferLength);
            checkForError(hr, "Could not get buffer from sample", PLUS_FILE_LINE);
            int _myAudioNumberOfChannels = 2;
            int numFrames = myBufferLength/(getBytesPerSample(SF_S16)*_myAudioNumberOfChannels);
            // queue audio sample
            AudioBufferPtr myAudioBuffer;
            if (_myResampleContext) {
                numFrames = audio_resample(_myResampleContext,
                        (int16_t*)(_myResampledSamples.begin()),
                        (int16_t*)(myBuffer), _myAudioNumberOfChannels);
                myAudioBuffer = Pump::get().createBuffer(numFrames);
                myAudioBuffer->convert(_myResampledSamples.begin(), SF_S16, _myAudioNumberOfChannels);
            } else {
                myAudioBuffer = Pump::get().createBuffer(numFrames);
                myAudioBuffer->convert(myBuffer, SF_S16, _myAudioNumberOfChannels);
            }
            _myAudioSink->queueSamples(myAudioBuffer);

            _myLastAudioTimeStamp = myTimeStamp;
        } else if (theOutputNumber != static_cast<DWORD>(_myAudioOutputId)) {
            AC_TRACE << "Unexpected output=" << theOutputNumber;
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    WMVDecoder::OnStatus(WMT_STATUS theStatus,
                         HRESULT hr,
                         WMT_ATTR_DATATYPE theType,
                         BYTE __RPC_FAR * theValue,
                         void __RPC_FAR * theContext)
    {
        asl::AutoLocker<ThreadLock> myLocker(_myLock); // protect _myEventResult, _myReadEOF
        switch (theStatus) {
        case WMT_LOCATING:
			{AC_INFO << "Locating.";}
            break;
        case WMT_CONNECTING:
			{AC_INFO << "Connecting.";}
            break;
        case WMT_BUFFERING_START:
			{AC_INFO << "Buffering Start.";}
            _myEventResult = hr;
            SetEvent(_myEvent);
            break;
        case WMT_BUFFERING_STOP:
			{AC_INFO << "Buffering Stop.";}
            break;
        case WMT_OPENED:
			{AC_INFO << "Opened File.";}
            _myEventResult = hr;
            SetEvent(_myEvent);
            break;
        case WMT_STARTED:
			{AC_INFO << "Started.";}
            _myEventResult = hr;
            SetEvent(_myEvent);
            break;
        case WMT_STOPPED:
			{AC_INFO << "Stopped.";}
            if (_myAudioSink) {
                _myAudioSink->stop();
            }
            _myEventResult = hr;
            SetEvent(_myEvent);
            break;
        case WMT_CLOSED:
			{AC_INFO << "Closed File.";}
            if (_myAudioSink) {
                _myAudioSink->stop();
            }
            _myEventResult = hr;
            SetEvent(_myEvent);
            break;
        case WMT_END_OF_STREAMING:
			{AC_INFO << "End of Streaming.";}
            break;
        case WMT_EOF:
			{AC_INFO << "End of File.";}
            _myReadEOF = true;
            _myEventResult = hr;
            SetEvent(_myEvent);
            break;
        default:
			{AC_TRACE << "OnStatus: Unhandled status=" << theStatus << " result=" << hr;}
            break;
        }
        return S_OK;
    }

    // Does not work for string attributes
    template <class T>
    void
    WMVDecoder::getStreamAttribute(IWMHeaderInfo * theHeaderInfo, const std::string & theName, T & theResult) {
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
                throw WindowsMediaException(string("Stream-Attribute ") + theName + " has unimplemented WMT_TYPE_STRING type", PLUS_FILE_LINE);
                break;
            case WMT_TYPE_BINARY:
                // XXX: Implement conversion
                throw WindowsMediaException(string("Stream-Attribute ") + theName + " has unimplemented WMT_TYPE_BINARY type", PLUS_FILE_LINE);
                break;
            case WMT_TYPE_BOOL:
                sourceType = &typeid(bool);
                break;
            case WMT_TYPE_WORD:
                sourceType = &typeid(WORD);
                break;
            case WMT_TYPE_GUID:
                // XXX: Implement conversion
                throw WindowsMediaException(string("Stream-Attribute ") + theName + " has unimplemented WMT_TYPE_GUID type", PLUS_FILE_LINE);
                break;
        }
        if (*sourceType != typeid(T)) {
            throw WindowsMediaException(string("Stream-Attribute ") + theName + " has wrong type " + sourceType->name() + ". Should be: " + typeid(T).name(), PLUS_FILE_LINE);
        }
        hr = theHeaderInfo->GetAttributeByName(&myStreamNumber, myWideName.c_str(), &myType, (BYTE*)&theResult, &myValueLength);
        if (hr == ASF_E_NOTFOUND) {
            throw WindowsMediaException(string("Stream-Attribute ") + theName + " was not found", PLUS_FILE_LINE);
        } else {
            checkForError(hr, "Could not get Attribute from video stream", PLUS_FILE_LINE);
        }
    }

    HRESULT WMVDecoder::setupMultiChannel() {
        //Set up multichannel playback
        BOOL fEnableDiscreteOutput = TRUE;
        DWORD dwSpeakerConfig = DSSPEAKER_5POINT1;
        HRESULT hr = E_FAIL;

        if (!_myReader) {
            AC_ERROR << "No Reader in setupMultiChannel";
            return E_FAIL;
        }

        CComQIPtr<IWMReaderAdvanced2, &IID_IWMReaderAdvanced2>
            pReaderAdvanced2(_myReader);
        if(! pReaderAdvanced2) {
            AC_ERROR << "Could not cast to Advanced Reader";
            return E_FAIL;
        }
        //Make the required settings on the Reader Object
        hr = pReaderAdvanced2->SetOutputSetting(0,
            g_wszEnableDiscreteOutput,
            WMT_TYPE_BOOL,
            (BYTE *)&fEnableDiscreteOutput,
            sizeof( BOOL ) );
        if(FAILED(hr)) {
            AC_ERROR << "Failed to SetOutputSetting g_wszEnableDiscreteOutput";
            return hr;
        }

        hr = pReaderAdvanced2->SetOutputSetting(0,
            g_wszSpeakerConfig,
            WMT_TYPE_DWORD,
            (BYTE *)&dwSpeakerConfig,
            sizeof( DWORD ) );
        if(FAILED(hr))
        {
            AC_TRACE << "Failed to SetOutputSetting g_wszSpeakerConfig";
            return hr;
        }

        // Dynamic range control can also be set using SetOutputSetting,
        // although we don't show that here.

        // Get the various formats supported by the audio output.
        // In this example, to keep things simple, we only handle
        // audio-only files with a single stream. In other words, we
        // assume that there is one audio output and that its number is zero.
        DWORD dwAudioOutput = 0;
        DWORD formats = 0;
        hr = _myReader->GetOutputFormatCount(dwAudioOutput, &formats);
        if(FAILED(hr))
        {
            AC_TRACE << "Failed to get output format count.";
            return hr;
        }

        // Multichannel formats, if available, are returned first
        for(DWORD j = 0; j < formats;j++)
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
                AC_ERROR << "Not enough core";
                hr = E_OUTOFMEMORY;
           }

            hr = pProps->GetMediaType( pNativeType, &cbFormat );
            if( hr != S_OK )
            {
                AC_ERROR << "Failed getting the media type";
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
                    AC_ERROR << "Failed to set output props";
                    return hr;
                }

                hr = _myReader->SetOutputProps(0, pProps);
                if( hr != S_OK )
                {
                    AC_ERROR << "Failed to set output props";
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
