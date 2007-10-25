//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _DShowHelper_H_
#define _DShowHelper_H_

#include <dshow.h>

#include <string>

namespace y60 {

void checkForDShowError(HRESULT hr, const std::string & theMessage, 
        const std::string & theFileLine);

} // namespace

#endif 
