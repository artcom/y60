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
            AC_ERROR << "Unsupported audio buffer type " << mySampleFormat << " in asl::createAudioBuffer()";
			return 0;}
    }
}

}
