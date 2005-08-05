//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "GLContextRegistry.h"
#include "AbstractRenderWindow.h"
#include <asl/Logger.h>

using namespace jslib;
using namespace asl;
using namespace y60;

namespace jslib {


void 
GLContextRegistry::registerContext(asl::WeakPtr<AbstractRenderWindow> theRenderWindow) {
    _myContexts.insert(theRenderWindow);
    AC_DEBUG << "Context registered with ContextRegistry, size: " << _myContexts.size();
}

void 
GLContextRegistry::unregisterContext(asl::WeakPtr<AbstractRenderWindow> theRenderWindow) {
    _myContexts.erase(theRenderWindow);
    //cleanup();
    AC_DEBUG << "Context removed from ContextRegistry, size: " << _myContexts.size();
}

Ptr<AbstractRenderWindow>
GLContextRegistry::getContext() {
    cleanup();
    for (ContextSet::iterator i = _myContexts.begin(); i != _myContexts.end(); ++i) {
        if (*i) {
            WeakPtr<AbstractRenderWindow> myWeaky = *i;
            return myWeaky.lock();
        } else {
            // TODO: We should remove it from the list for performance
        }
    }
    return Ptr<AbstractRenderWindow>(0);
}

void
GLContextRegistry::cleanup() {
    int myInitialSize = _myContexts.size();
    //_myContexts.erase(asl::WeakPtr<AbstractRenderWindow>(0));
}

} // namespace jslib

