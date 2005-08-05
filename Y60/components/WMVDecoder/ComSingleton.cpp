//=============================================================================
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "ComSingleton.h"

#include <objbase.h>
#include <asl/Logger.h>

namespace asl {

    void
    ComSingleton::ref() {
        if (_myDecoderCount++ == 0) {
            AC_DEBUG << "ComSingleton CoInitialize";
            HRESULT hr = CoInitialize(NULL);
            if (hr != S_OK) {
                AC_FATAL << "CoInitialize failed";
            }
        }
    }

    void
    ComSingleton::unref() {
        if (--_myDecoderCount == 0) {
            AC_DEBUG << "ComSingleton CoUninitialize";
            CoUninitialize();
        }
    }
}
