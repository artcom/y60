//=============================================================================
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_WMADecoder_h_
#define _ac_y60_WMADecoder_h_

#include <audio/BufferedSource.h>
#include <asl/Exception.h>
#include <wmsdk/wmsdk.h>

namespace y60 {

    DEFINE_EXCEPTION(WindowsMediaAudioException, asl::Exception);

    /**
     * @ingroup y60WMADecoder
     * Decoder class for Windows Media Audio files.
     *
     * \par requirements
     * win98 or later\n
     *
     */
    class WMADecoder :
        public IWMReaderCallback,
        public AudioBase::BufferedSource
    {
	    static const MAX_TIMEOUT_FOR_EVENT = 15000;

        public:
            WMADecoder(const std::string & theFilename, double theVolume = 1.0, unsigned theMaxChannels = 2);
            virtual ~WMADecoder();

            // IFileReader
	        double getDuration() const;

            // AudioAsyncModule
            void stop();
            void pause();

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

        private:
            LONG          _myReferenceCount;
            IWMReader *   _myReader;
            HANDLE        _myEvent;
            HRESULT       _myEventResult;
            WORD          _myAudioOutputId;
            unsigned      _myMaxChannels;

            /**
             * Retrieves a metadata attribute
             * \param theHeaderInfo pointer to IWMHeaderInfo structure as retrieved from system
             * \param theName name of the Attribute to retrieve
             * \param theResult out parameter for result
             * \todo implement handling of string and binary types
             */
            template <class T>
            void getStreamAttribute(IWMHeaderInfo * theHeaderInfo, const std::string & theName, T & theResult);

            bool waitForEvent(unsigned theWaitTime = MAX_TIMEOUT_FOR_EVENT);

            void load(const std::string & theUrl);
            void setupAudio(const std::string & theUrl);

            /**
             * Sets up Multichannel decoding for audio
             * \return S_OK when Multichannel decoding is enabled for all formats, else some error HRESULT
             */
            HRESULT setupMultiChannel();
    };
}
#endif
