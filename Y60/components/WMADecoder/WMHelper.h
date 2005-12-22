//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _WMHelper_H_
#define _WMHelper_H_

#include <dsound.h>  // For the DSSPEAKER_5POINT1 value
#include <mmreg.h>   // For WAVEFORMATEXTENSIBLE (if you use it)

#include <string>

namespace y60 {

void checkForWMError(HRESULT hr, const std::string & theMessage, 
        const std::string & theFileLine);

} // namespace

#endif 
