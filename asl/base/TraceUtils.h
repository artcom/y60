//==============================================================================
//
// Copyright (C) 2007, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================

#ifndef TRACE_UTILS_INCLUDED
#define TRACE_UTILS_INCLUDED

#include <string>

struct StackFrameBase {
    ptrdiff_t frame;
    std::string name;
};

std::string beautify(const std::string & theSymbol);

#endif // TRACE_UTILS_INCLUDED

