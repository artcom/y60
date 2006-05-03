//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: WMVDecoder.h,v $
//   $Author: thomas $
//   $Revision: 1.10 $
//   $Date: 2005/04/19 09:28:22 $
//
//  Description: A simple scene class.
//
//=============================================================================

#ifndef _ac_WMVDecoder_h_
#define _ac_WMVDecoder_h_

#include <y60/AsyncDecoder.h>
#include <y60/FFMpegDecoder.h>
#include <y60/MovieEncoding.h>
#include <asl/PlugInBase.h>
#include <asl/ThreadLock.h>
#include <asl/MappedBlock.h>
#include <asl/Exception.h>

#include <windows.h>
#include <wmsdk/wmsdk.h>
#include <deque>

namespace y60 {
    DEFINE_EXCEPTION(WindowsMediaException, asl::Exception);

    const std::string MIME_TYPE_WMV = "application/wmv";
    const std::string MIME_TYPE_AVI = "application/avi";

    /**
     * @ingroup Y60componentsWMVDecoder
     *
     * @par requirements
     * win98 or later\n
     * windows media sdk\n
     *
     * Videodecoder for WMV files. It decodes video files using the microsoft
     * decoder. Video and Audio are cached internally. Audio is played back
     * using _myAudioBufferedSource. The Decoder spawns multiple threads which
     * call OnSample and OnStatus asynchronously.
     */
    class WMVDecoder :
        public y60::AsyncDecoder,
        public IWMReaderCallback,
        public asl::PlugInBase
    {

        // INSSBuffer with timestamp
        class VideoFrame {
        public:
            VideoFrame(double theTimestamp, INSSBuffer __RPC_FAR & theBuffer);
            ~VideoFrame();

            inline double getTimestamp() const {
                return _myTimestamp;
            }
            inline INSSBuffer __RPC_FAR * getBuffer() {
                return _myBuffer;
            }

        private:
            double _myTimestamp;
            INSSBuffer __RPC_FAR * _myBuffer;
        };
        typedef asl::Ptr<VideoFrame> VideoFramePtr;
        typedef std::deque<VideoFramePtr> VideoFrameCache;

    public:
        WMVDecoder(asl::DLHandle theDLHandle);
        virtual ~WMVDecoder();
        virtual asl::Ptr<MovieDecoderBase> instance() const;

        //
        // Methods of IUnknown
        //
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject);
        ULONG STDMETHODCALLTYPE AddRef();
        ULONG STDMETHODCALLTYPE Release();

        //
        // Methods of IWMReaderCallback
        //
        HRESULT STDMETHODCALLTYPE OnSample(DWORD theOutputNumber,
                                        QWORD theSampleTime,
                                        QWORD theSampleDuration,
                                        DWORD theFlags,
                                        INSSBuffer __RPC_FAR *theSample,
                                        void __RPC_FAR *theContext);

        HRESULT STDMETHODCALLTYPE OnStatus(WMT_STATUS theStatus,
                                        HRESULT hr,
                                        WMT_ATTR_DATATYPE theType,
                                        BYTE __RPC_FAR *theValue,
                                        void __RPC_FAR *theContext);

        //
        // Methods of MovieDecoderBase
        //
        double getAudioVideoDelay() const;
        std::string canDecode(const std::string & theUrl, asl::ReadableStream * theStream = 0);

        void load(const std::string & theUrl);

        /**
         * loads a movie from the stream given by theSource
         * @param theFilename file to identify theSource
         */
        void load(asl::Ptr<asl::ReadableStream> theSource, const std::string & theFilename) {
            throw asl::NotYetImplemented(JUST_FILE_LINE);
        }

        /**
         * reads a frame.
         * @param theTime time of the frame to deliver
         * @param theFrame is ignored by this decoder
         * @param theTargetRaster image raster to render the frame into
         * @return timestamp of the frame delivered in theTargetRaster
         */
        double readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster);

        void startMovie(double theStartTime = 0.0f);
        void resumeMovie(double theStartTime = 0.0f);
        void stopMovie();
        void pauseMovie();
        void closeMovie();

        const char * getName() const { return "y60WMVDecoder"; }
    private:
        static const MAX_TIMEOUT_FOR_EVENT = 60000;

        bool checkNetworkConfig();
        const std::string getProxyForProtocol(IWMReaderNetworkConfig * theNetworkConfig,
                                              const std::wstring & theWideProtocol);
        void setupNetworkConfig(IWMReaderNetworkConfig * theNetworkConfig, const std::string & theProtocol);

        /**
         * Retrieves a metadata attribute
         * @param theHeaderInfo pointer to IWMHeaderInfo structure as retrieved from system
         * @param theName name of the attribute to retrieve
         * @param theResult template output parameter
         * @todo implement handling of string and binary types
         */
        template <class T>
        void getStreamAttribute(IWMHeaderInfo * theHeaderInfo, const std::string & theName, T & theResult);

        void setupVideoAndAudio(const std::string & theUrl);

        // Wait for event signal from decoder thread.
        bool waitForEvent(unsigned theWaitTime = MAX_TIMEOUT_FOR_EVENT);
        void resetEvent();

        /**
         * Empties the sample queue. Call this to empty the internal cache
         */
        void releaseBuffers();
        HRESULT setupMultiChannel();

        //asl::ThreadLock _myLock;

        unsigned        _myFrameCacheSize;
        VideoFrameCache _myFrameCache;

        int           _myAudioOutputId;
        int           _myVideoOutputId;
        LONG          _myReferenceCount;
        IWMReader *   _myReader;
        HANDLE        _myEvent;
        HRESULT       _myEventResult;

        double	      _myCurrentPlaySpeed;
        double        _myFrameRate;

        bool          _myFirstFrameDelivered;
        double        _myLastVideoTimeStamp;
        double        _myLastAudioTimeStamp;
        double        _myAudioVideoDelay;
        bool          _myCachingFlag;
        
        ReSampleContext *   _myResampleContext;
        unsigned      _myAudioNumberOfChannels;

        static asl::Block     _mySamples;
        static asl::Block     _myResampledSamples;
        bool _myReadEOF;
        asl::ThreadLock _myLock;
    };
    typedef asl::Ptr<WMVDecoder> WMVDecoderPtr;
}

#endif
