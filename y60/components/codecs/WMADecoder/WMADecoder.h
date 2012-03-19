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

#ifndef _WMADecoder2_H_
#define _WMADecoder2_H_

#include <y60/sound/IAudioDecoder.h>
#include <y60/sound/SoundManager.h>

#include <asl/audio/ISampleSink.h>
#include <asl/base/Block.h>

#include <wmsdk.h>

#pragma warning(push,1)
extern "C" {
#   include <avformat.h>
}
#pragma warning(pop)

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
