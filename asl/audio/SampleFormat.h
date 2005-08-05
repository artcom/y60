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

#ifndef INCL_SAMPLEFORMAT
#define INCL_SAMPLEFORMAT

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

unsigned getBytesPerSample(SampleFormat theSampleFormat);
std::ostream & operator<<(std::ostream & s, SampleFormat SF); 

}

#endif //_SampleFormat_H_
