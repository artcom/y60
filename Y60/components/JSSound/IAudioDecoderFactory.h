//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _IAudioDecoderFactory_H_
#define _IAudioDecoderFactory_H_

#include "IAudioDecoder.h"

#include <asl/ISampleSink.h>

#include <string>

namespace y60 {

DEFINE_EXCEPTION(DecoderException, asl::Exception);
DEFINE_EXCEPTION(FileNotFoundException, asl::Exception);

class IAudioDecoderFactory
{
    public:
        // Throws a decoder exception if not successful.
        virtual IAudioDecoder* tryCreateDecoder(std::string myURI) = 0;
};

} // namespace

#endif 
