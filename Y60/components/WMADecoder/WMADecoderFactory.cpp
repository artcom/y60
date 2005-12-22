//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "WMADecoderFactory.h"
#include "WMADecoder.h"
#include "WMHelper.h"

#include <y60/SoundManager.h>

#include <asl/ISampleSink.h>
//#include <asl/Singleton.h>
#include <asl/Block.h>
#include <asl/file_functions.h>

using namespace asl;

extern "C"
EXPORT asl::PlugInBase * y60WMADecoder_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::WMADecoderFactory(myDLHandle);
}

namespace y60 {

WMADecoderFactory::WMADecoderFactory(asl::DLHandle theDLHandle)
    : PlugInBase(theDLHandle)
{
    AC_DEBUG << "WMADecoderFactory::WMADecoderFactory";
    HRESULT hr = CoInitialize(NULL);
    checkForWMError(hr, "CoInitialize failed", PLUS_FILE_LINE);
}

WMADecoderFactory::~WMADecoderFactory() {
    AC_DEBUG << "WMADecoderFactory::~WMADecoderFactory";
    SoundManager& mySoundManager = Singleton<SoundManager>::get();
    mySoundManager.unregisterDecoderFactory(this);
    CoUninitialize();
}

IAudioDecoder* WMADecoderFactory::tryCreateDecoder(const std::string& myURI) {
    AC_DEBUG << "WMADecoderFactory::tryCreateDecoder (" << myURI << ")";

    // Throws DecoderException if the file can't be decoded.
    return new WMADecoder(myURI);
}

int WMADecoderFactory::getPriority() const {
    return 0;
}

void WMADecoderFactory::_initializeSingletonManager
        (SingletonManager* theSingletonManager) 
{
    PlugInBase::_initializeSingletonManager(theSingletonManager);

    // This should really happen in the constructor, but the singleton
    // manager isn't initialized until now.
    SoundManager& mySoundManager = Singleton<SoundManager>::get();
    mySoundManager.registerDecoderFactory(this);
}

} // namespace

