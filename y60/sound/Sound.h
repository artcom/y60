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

#ifndef _Sound_H_
#define _Sound_H_

#include "y60_sound_settings.h"

#include "IAudioDecoder.h"
#include "SoundCacheItem.h"

#include <asl/audio/HWSampleSink.h>
#include <asl/base/Stream.h>
#include <asl/audio/ISampleSink.h>
#include <asl/base/ThreadLock.h>

namespace y60 {

class Sound;

typedef asl::Ptr<Sound> SoundPtr;
typedef asl::WeakPtr<Sound> SoundWeakPtr;

DEFINE_EXCEPTION(SoundException, asl::Exception);

class Y60_SOUND_DECL Sound :
    public asl::TimeSource, public asl::ISampleSink
{
    public:
        Sound (std::string myURI,  IAudioDecoder * myDecoder, SoundCacheItemPtr myCacheItem,
                bool myLoop = false);
        virtual ~Sound();
        void setSelf(const SoundPtr& mySelf);
        void play ();
        void pause ();
        void stop ();
        void setVolume (float theVolume);
        void setVolumes(float theLeftVolume, float theRightVolume);
        void fadeToVolume (float Volume, asl::Time theTime);
        double getVolume () const;
        std::string getName() const;
        asl::Time getDuration() const;
        asl::Time getCurrentTime();
        bool isLooping() const;
        void setLooping(bool theLoop);
        void seek (asl::Time thePosition);
        void seekRelative (double theAmount);
        asl::Time getBufferedTime() const;
        bool isPlaying() const;
        unsigned getNumUnderruns() const;
        enum CacheState {CACHING, CACHED, NOCACHE};
        CacheState getCacheState() const;

        // Interface to Media.
        void update(double theTimeSlice);

        // ISampleSink interface (to Decoder)
        bool queueSamples(asl::AudioBufferPtr& theBuffer);

        static int getNumSoundsAllocated();

    private:
        void open();
        void close();
        bool decode();

        mutable asl::ThreadLock _myLock;

        std::string _myURI;
        asl::HWSampleSinkPtr _mySampleSink;
        IAudioDecoder * _myDecoder;

        SoundWeakPtr _mySelf;
        SoundPtr _myLockedSelf;

        bool _myDecodingComplete;
        bool _myIsLooping;

        asl::Time _myTargetBufferedTime;
        asl::Time _myMaxUpdateTime; // Max. time to prefetch per update.

        SoundCacheItemPtr _myCacheItem; // This is 0 if we're not caching.

        static asl::ThreadLock _mySoundsAllocatedLock;
        static int _myNumSoundsAllocated;
};

} // namespace

#endif
