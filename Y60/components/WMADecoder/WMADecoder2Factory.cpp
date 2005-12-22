//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "WMADecoder2Factory.h"
#include "WMADecoder2.h"
#include "WMHelper.h"

#include <y60/SoundManager.h>

#include <asl/ISampleSink.h>
//#include <asl/Singleton.h>
#include <asl/Block.h>
#include <asl/file_functions.h>

using namespace asl;

extern "C"
EXPORT asl::PlugInBase * y60WMADecoder2_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::WMADecoder2Factory(myDLHandle);
}

namespace y60 {

WMADecoder2Factory::WMADecoder2Factory(asl::DLHandle theDLHandle)
    : PlugInBase(theDLHandle)
{
    AC_DEBUG << "WMADecoder2Factory::WMADecoder2Factory";
    HRESULT hr = CoInitialize(NULL);
    checkForWMError(hr, "CoInitialize failed", PLUS_FILE_LINE);
}

WMADecoder2Factory::~WMADecoder2Factory() {
    AC_DEBUG << "WMADecoder2Factory::~WMADecoder2Factory";
    SoundManager& mySoundManager = Singleton<SoundManager>::get();
    mySoundManager.unregisterDecoderFactory(this);
    CoUninitialize();
}

IAudioDecoder* WMADecoder2Factory::tryCreateDecoder(const std::string& myURI) {
    AC_DEBUG << "WMADecoder2Factory::tryCreateDecoder (" << myURI << ")";

    // Throws DecoderException if the file can't be decoded.
    return new WMADecoder2(myURI);
}

int WMADecoder2Factory::getPriority() const {
    return 0;
}

void WMADecoder2Factory::_initializeSingletonManager
        (SingletonManager* theSingletonManager) 
{
    PlugInBase::_initializeSingletonManager(theSingletonManager);

    // This should really happen in the constructor, but the singleton
    // manager isn't initialized until now.
    SoundManager& mySoundManager = Singleton<SoundManager>::get();
    mySoundManager.registerDecoderFactory(this);
}

} // namespace

