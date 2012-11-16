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

#ifndef _IAudioDecoder_H_
#define _IAudioDecoder_H_

#include "y60_sound_settings.h"

#include <asl/audio/ISampleSink.h>
#include <asl/base/Time.h>

namespace y60 {

class Y60_SOUND_DECL IAudioDecoder
{
    public:
        virtual ~IAudioDecoder() {};

        virtual void seek(asl::Time thePosition) = 0;
        virtual unsigned getSampleRate() = 0;
        virtual unsigned getNumChannels() = 0;
        virtual asl::Time getDuration() const = 0;
        virtual std::string getName() const = 0;
        virtual void setSampleSink(asl::ISampleSink* mySampleSink) = 0;
        virtual void setTime(asl::Time myTime) {};
        virtual unsigned getCurFrame() const = 0; // This is the total number of frames that
                                 // have been decoded. It doesn't take into account seeks etc.
        virtual void decodeEverything() = 0;

        // There are two types of decoders. Sync decoders don't have their own
        // decoding thread. Buffering is handled externally - the Decoder client
        // calls decode() until enough data has been buffered.
        // Async decoders have an internal thread that decodes the data and do their
        // own buffering, so they need to know when playback is running.
        virtual bool isSyncDecoder() const = 0;

        // Sync decoder methods
        virtual bool decode() { return false; };

        // Async decoder methods
        virtual void play() {};
        virtual void stop() {};
        virtual void pause() {};
        virtual bool isEOF() const { return false; };
};

} // namespace

#endif
