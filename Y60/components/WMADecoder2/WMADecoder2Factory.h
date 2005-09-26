//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _WMADecoder2Factory_H_
#define _WMADecoder2Factory_H_

#include <y60/IAudioDecoderFactory.h>

#include <asl/ISampleSink.h>
#include <asl/Block.h>
#include <asl/PlugInBase.h>

namespace y60 {

const std::string MIME_TYPE_WMA = "audio/wma";
const std::string MIME_TYPE_MP3 = "audio/mp3";

class WMADecoder2Factory: public IAudioDecoderFactory, public asl::PlugInBase
{
    public:
        WMADecoder2Factory(asl::DLHandle theDLHandle);
        virtual ~WMADecoder2Factory();
        virtual IAudioDecoder* tryCreateDecoder(const std::string & myURI);
        virtual int getPriority() const; 

    protected:
	    void _initializeSingletonManager(asl::SingletonManager* theSingletonManager);

    private:
        bool canDecode(const std::string & theUrl) const;
};

} // namespace

#endif 

