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

#include "SampleFormat.h"

#include <asl/base/Assure.h>

namespace asl {

unsigned getBytesPerSample(SampleFormat theSampleFormat)
{
    switch(theSampleFormat) {
        case SF_S8:
        case SF_U8:
            return 1;
        case SF_S16:
        case SF_U16:
            return 2;
        case SF_S24:
        case SF_U24:
            return 3;
        case SF_S32:
        case SF_U32:
        case SF_F32:
            return 4;
        default:
            ASSURE_MSG(false, "getBytesPerSample: Unknown sample format");
            return 0;
    }
}

std::ostream & operator<<(std::ostream & s, SampleFormat SF) {
    switch(SF) {
        case SF_S8:
            return s << "SF_S8";
        case SF_U8:
            return s << "SF_U8";
        case SF_S16:
            return s << "SF_S16";
        case SF_U16:
            return s << "SF_U16";
        case SF_S24:
            return s << "SF_S24";
        case SF_U24:
            return s << "SF_U24";
        case SF_S32:
            return s << "SF_S32";
        case SF_U32:
            return s << "SF_U32";
        case SF_F32:
            return s << "SF_F32";
        default:
            return s << "Unknown sample format";
    }
}
    
}

