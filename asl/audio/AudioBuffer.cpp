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

#include "AudioBuffer.h"

#include <asl/base/Logger.h>

namespace asl {

template<>
SampleFormat AudioBuffer<Signed16>::getSampleFormat() const {
    return SF_S16;
}

template<>
SampleFormat AudioBuffer<float>::getSampleFormat() const {
    return SF_F32;
}
    
AudioBufferBase * createAudioBuffer(SampleFormat mySampleFormat, unsigned numFrames, 
        unsigned numChannels, unsigned mySampleRate)
{
    switch (mySampleFormat) {
        case SF_S16:
            return new AudioBuffer<Signed16>(numFrames, numChannels, mySampleRate);
        case SF_F32:
            return new AudioBuffer<float>(numFrames, numChannels, mySampleRate);
		default:{
            AC_ERROR << "Unsupported audio buffer type in createAudioBuffer().";
			return 0;}
    }
}

}
