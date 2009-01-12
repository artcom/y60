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

#ifndef INCL_SAMPLEFORMAT
#define INCL_SAMPLEFORMAT

#include "asl_audio_settings.h"

#include <iostream>

namespace asl {

enum SampleFormat
{
    SF_S8,
    SF_S16,
    SF_S24,
    SF_S32,
    SF_U8,
    SF_U16,
    SF_U24,
    SF_U32,
    SF_F32
};

ASL_AUDIO_EXPORT unsigned getBytesPerSample(SampleFormat theSampleFormat);
ASL_AUDIO_EXPORT std::ostream & operator<<(std::ostream & s, SampleFormat SF); 

}

#endif //_SampleFormat_H_
