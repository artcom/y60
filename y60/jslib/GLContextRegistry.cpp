/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "GLContextRegistry.h"
#include "AbstractRenderWindow.h"
#include <asl/base/Logger.h>

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

asl::Ptr<AbstractRenderWindow>
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
    return asl::Ptr<AbstractRenderWindow>(0);
}

void
GLContextRegistry::cleanup() {
    //int myInitialSize = _myContexts.size();
    //_myContexts.erase(asl::WeakPtr<AbstractRenderWindow>(0));
}

} // namespace jslib

