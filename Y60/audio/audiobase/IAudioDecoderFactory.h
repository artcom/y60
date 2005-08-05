//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: IAudioDecoderFactory.h,v $
//
//     $Author: build $
//
//   $Revision: 1.2 $
//
//
// Description:
//
//
//=============================================================================

#ifndef _ac_AudioBase_IAudioDecoderFactory_h_
#define _ac_AudioBase_IAudioDecoderFactory_h_

#include "IFileReader.h"
#include <y60/IDecoder.h>

namespace AudioBase {

    class IAudioDecoderFactory : public y60::IDecoder {
        public:
            virtual std::string canDecode(const std::string & theUrl, asl::ReadableStream * theStream = 0) = 0;
            virtual IFileReader * instance(const std::string & theName, double theVolume = 1.0) const = 0;
    };
}

#endif
