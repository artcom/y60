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

#include "SampleFormat.h"

#include <asl/Assure.h>

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

