//=============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//=============================================================================

#ifndef INCL_HWSAMPLESINK
#define INCL_HWSAMPLESINK

#include "ISampleSink.h"
#include "SampleFormat.h"
#include "AudioBufferBase.h"
#include "VolumeFader.h"
#include "AudioTimeSource.h"

#include <asl/Time.h>
#include <asl/Block.h>
#include <asl/ThreadLock.h>
#include <asl/settings.h>

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
    
class HWSampleSink: public AudioTimeSource, public ISampleSink
{
    public:
        enum State {STOPPED,            // No sound is playing.
                    RUNNING,            // Sound is playing.
                    STOPPING_FADE_OUT,  // Client has sent stop cmd, fadeout in progress.
                    PAUSING_FADE_OUT,   // Client has sent pause cmd, fadeout in progress.
                    PAUSED,             // Sound is paused.
                    PLAYBACK_DONE       // All buffers have been played, _myStopWhenEmpty
                                        // is set.
        };

        // Possible state transitions:
        // Source State       Command    
        //                    play()    stop()              pause()
        // STOPPED            RUNNING   ignored             ignored
        // RUNNING            ignored   STOPPING_FADE_OUT   PAUSING_FADE_OUT
        // STOPPING_FADE_OUT  RUNNING   ignored             ignored
        // PAUSING_FADE_OUT   RUNNING   STOPPING_FADE_OUT   ignored
        // PAUSED             RUNNING   STOPPED             ignored

        HWSampleSink(const std::string & myName, SampleFormat mySampleFormat, 
                unsigned mySampleRate, unsigned numChannels);
        virtual ~HWSampleSink();
        void setSelf(const HWSampleSinkPtr& mySelf);
        virtual void play();
        virtual void pause();
        virtual void stop(bool theRunUntilEmpty = false);
        virtual void setVolume(float theVolume);
        virtual void fadeToVolume(float theVolume, float theTime);
        virtual float getVolume() const;
        bool isPlaying() const;
        bool queueSamples(AudioBufferPtr& theBuffer);
        asl::Time getBufferedTime() const;
        std::string getName() const;
        State getState() const;
        static std::string stateToString(State theState);
        unsigned getNumUnderruns() const;

        virtual void dumpState();

        // Interface to ALSAPump/DSSampleSink
        void deliverData(AudioBufferBase& theBuffer);
    
        void lock();
        void unlock();

        unsigned getNumChannels() const;
        unsigned getSampleRate() const;
        unsigned getBytesPerSecond() const;
        SampleFormat getSampleFormat() const;
        unsigned getBytesPerFrame() const;

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
        
        std::string  _myName;
        unsigned _mySampleRate;
        SampleFormat _mySampleFormat;
        unsigned _numChannels;

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
};

}

#endif 
