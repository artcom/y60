/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
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

#ifndef INCL_HWSAMPLESINK
#define INCL_HWSAMPLESINK

#include "asl_audio_settings.h"

#include "ISampleSink.h"
#include "AudioTimeSource.h"
#include "SampleSource.h"
#include "VolumeFader.h"
#include "ExternalTimeSource.h"

#include <asl/base/Time.h>
#include <asl/base/Block.h>
#include <asl/base/ThreadLock.h>
#include <asl/base/settings.h>

#include <string>
#include <list>

namespace asl {

    // #define USE_DASHBOARD
    
    class Pump;
    class HWSampleSink;

    // Use this pointer as smart pointer to HWSampleSinks. If you use the default Ptr,
    // you'll get thread-specific free lists. Since lots of pointers are allocated 
    // in one thread and deleted in another, that will cause memory leaks. The 
    // PtrHeapAllocator used here is slower but works.
    typedef Ptr<HWSampleSink, MultiProcessor, PtrHeapAllocator<MultiProcessor> > HWSampleSinkPtr;
    typedef WeakPtr<HWSampleSink, MultiProcessor, PtrHeapAllocator<MultiProcessor> > HWSampleSinkWeakPtr;
    
    class ASL_AUDIO_EXPORT HWSampleSink: public SampleSource, public AudioTimeSource, public ISampleSink
    {
    public:
        enum State {STOPPED,            // No sound is playing.
                    RUNNING,            // Sound is playing.
                    STOPPING_FADE_OUT,  // Client has sent stop cmd, fadeout in progress.
                    PAUSING_FADE_OUT,   // Client has sent pause cmd, fadeout in progress.
                    PAUSED,             // Sound is paused.
                    PLAYBACK_DONE,      // All buffers have been played, _myStopWhenEmpty
                                        // is set.
                    DELAYING_PLAY       // delayedPlay has been called, i.e. time is running
                                        // but playback hasn't started yet.
        };

        // Possible state transitions:
        // Source State       Command    
        //                    play()    stop()              pause()          delayedPlay()
        // STOPPED            RUNNING   ignored             ignored          RUNNING
        // RUNNING            ignored   STOPPING_FADE_OUT   PAUSING_FADE_OUT illegal
        // STOPPING_FADE_OUT  RUNNING   ignored             ignored          not implemented   
        // PAUSING_FADE_OUT   RUNNING   STOPPING_FADE_OUT   ignored          illegal
        // PAUSED             RUNNING   STOPPED             ignored          illegal

        HWSampleSink(const std::string & myName, SampleFormat mySampleFormat, 
                     unsigned mySampleRate, unsigned numChannels);
        virtual ~HWSampleSink();
        void setSelf(const HWSampleSinkPtr& mySelf);
        virtual void play();
        virtual void pause();
        virtual void stop(bool theRunUntilEmpty = false);
        virtual void delayedPlay(asl::Time theTimeToStart);
        virtual void setVolume(float theVolume);
        virtual void fadeToVolume(float theVolume, float theTime);
        virtual float getVolume() const;
        bool isPlaying() const;
        bool queueSamples(AudioBufferPtr& theBuffer);
        asl::Time getBufferedTime() const;
        asl::Time getPumpTime() const;
        //        std::string getName() const;
        State getState() const;
        static std::string stateToString(State theState);
        unsigned getNumUnderruns() const;

        virtual void dumpState();

        // Interface to ALSAPump/DSSampleSink
        virtual void deliverData(AudioBufferBase& theBuffer);
    
        void lock();
        void unlock();

        //         unsigned getNumChannels() const;
        //         unsigned getSampleRate() const;
        //         unsigned getBytesPerSecond() const;
        //         SampleFormat getSampleFormat() const;
        //         unsigned getBytesPerFrame() const;

    private:
        void changeState(State newState);
        AudioBufferBase* getNextBuffer();
        void setMarker(AudioBufferBase& theBuffer, float theValue);
        // _myQueueLock is locked on write access to buffer queue and whenver data needed
        // to process the buffers (volume, pan etc.) is changed.
        mutable asl::ThreadLock _myQueueLock;  
        std::list < AudioBufferPtr > _myBufferQueue;
        unsigned _myPosInInputBuffer;
        AudioBufferPtr _myBackupBuffer; // Used when there is an underrun.
        unsigned _numUnderruns;
        bool _isUsingBackupBuffer;
        
        //         std::string  _myName;
        //         unsigned _mySampleRate;
        //         SampleFormat _mySampleFormat;
        //         unsigned _numChannels;

        State _myState;

        // We always keep a weak pointer to ouselves. While playing, we keep a smart
        // pointer, so you can forget the object from the outside and it'll keep
        // playing until it's done and self-destruct afterwards.
        HWSampleSinkWeakPtr _mySelf;
        HWSampleSinkPtr _myLockedSelf;

        Unsigned64 _myFrameCount;  // This allows for streams lasting about 13 million years 
                                   // if I didn't miscalculate :-).
        VolumeFaderPtr _myVolumeFader;
        float _myVolume;
        bool _myStopWhenEmpty;

        bool _isDelayingPlay;
        asl::Time _myTimeToStart;  // Used for delayedPlay().

        ExternalTimeSourcePtr _myPumpTimeSource;
    };

}

#endif 
