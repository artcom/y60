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
*/

#ifndef _SoundCacheItem_H_
#define _SoundCacheItem_H_

#include "y60_sound_settings.h"

#include <asl/base/Ptr.h>
#include <asl/base/Time.h>
#include <asl/audio/AudioBufferBase.h>
#include <asl/audio/ISampleSink.h>

#include <string>
#include <map>

namespace y60 {

class SoundCacheItem: public asl::ISampleSink
{
    public:
        SoundCacheItem(const std::string& myURI);
        virtual ~SoundCacheItem();
        std::string getURI() const;
        unsigned getMemUsed() const;
        bool isFull() const;
        void doneCaching(int theTotalFrames = -1);

        asl::AudioBufferPtr getBuffer(int theStartFrame) const;
        int getNumFrames() const;
        unsigned getSampleRate() const;
        unsigned getNumChannels() const;
        asl::Time getLastUsedTime() const;
        void incInUseCount();
        void decInUseCount();
        bool isInUse() const;

        // ISampleSink interface
        bool queueSamples(asl::AudioBufferPtr& theBuffer);

    private:
        std::string _myURI;
        // An int has room for 12 hours of samples at 48000 samples/sec.
        int _myFramesDecoded;
        int _myTotalFrames;
        typedef std::map<int, asl::AudioBufferPtr> BufferMap;
        BufferMap _myBuffers;

        asl::Time _myLastUseTime;
        int _myInUseCount;

};

typedef asl::Ptr<SoundCacheItem> SoundCacheItemPtr;

} // namespace

#endif
