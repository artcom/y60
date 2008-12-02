//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _WMADecoder2_H_
#define _WMADecoder2_H_

#include <y60/sound/IAudioDecoder.h>
#include <y60/sound/SoundManager.h>

#include <asl/audio/ISampleSink.h>
#include <asl/base/Block.h>

#include <wmsdk/wmsdk.h>

#ifdef OSX
    extern "C" {
#       include <libavformat/avformat.h>
    }
#   undef AV_NOPTS_VALUE
#   define AV_NOPTS_VALUE 0x8000000000000000LL
#else
#   if defined(_MSC_VER)
#       define EMULATE_INTTYPES
#       pragma warning(push,1)
#   endif
    extern "C" {
#       include <ffmpeg/avformat.h>
    }
#   if defined(_MSC_VER)
#       pragma warning(pop)
#   endif
#endif

namespace y60 {

class WMADecoder: public IAudioDecoder, public IWMReaderCallback
{
        static const unsigned int MAX_TIMEOUT_FOR_EVENT = 15000;
    public:
    
        WMADecoder (const std::string& myURI);
        virtual ~WMADecoder();

        virtual void play();
        virtual void stop();
        virtual void pause();
        virtual bool isEOF() const;
        virtual void seek (asl::Time thePosition);
        virtual unsigned getSampleRate();
        virtual unsigned getNumChannels();
        virtual asl::Time getDuration() const;
        std::string getName() const;
        virtual void setSampleSink(asl::ISampleSink* mySampleSink);
        virtual bool isSyncDecoder() const;
        virtual void setTime(asl::Time myTime);
        
        virtual unsigned getCurFrame() const; 
        virtual void decodeEverything();
        
        //
        // Methods of IUnknown
        //

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, 
                void __RPC_FAR *__RPC_FAR *ppvObject);
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
        void open();
        void close();

        void setupAudio();
        void preroll(asl::Time thePosition);
        void waitForSamples();
        bool waitForEvent(unsigned theWaitTime = MAX_TIMEOUT_FOR_EVENT);

        std::string _myURI;
        IWMReader * _myReader;
        HANDLE _myEvent;
        HANDLE _mySampleEvent;
        HRESULT _myEventResult;
        unsigned _myMaxChannels;
        WORD _myAudioOutputId;
        unsigned _mySampleRate;
        unsigned _myNumChannels;
        asl::ISampleSink* _mySampleSink;
        bool _myDecodingDone;
        bool _myUseUserClock;
        ReSampleContext * _myResampleContext;
        asl::Block _myResampledSamples;
        
        enum State {STOPPED, PAUSED, PLAYING};
        State _myState;

        LONG _myReferenceCount; // For QueryInterface etc.

        unsigned _myCurFrame;
};

} // namespace

#endif 
