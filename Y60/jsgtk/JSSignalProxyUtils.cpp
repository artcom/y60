//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


#include "JSSignalProxyUtils.h"
#include <asl/Logger.h>

namespace jslib {

void 
JSSignalAdapterBase::on_target_finalized(JSObject * theTarget, sigc::connection theConnection) {
    if (theConnection.connected()) {
        theConnection.disconnect();
    }
}

void 
JSSignalAdapterBase::on_target_finalized_dbg(JSObject * theTarget, sigc::connection theConnection, std::string theDebugMessage) {
    if (theConnection.connected()) {
        AC_TRACE << "finalizing JSObject " << theTarget << ", disconnecting " << theDebugMessage; 
        theConnection.disconnect();
    }
}
}

