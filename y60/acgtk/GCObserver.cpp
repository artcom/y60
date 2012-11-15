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

//own header
#include "GCObserver.h"

#include <asl/base/Logger.h>

namespace jslib {

using namespace std;

GCObserver::GCObserver() : _myContext(0), _myPrevCallback(0) {
    AC_TRACE << "CTOR" << endl;
}

GCObserver::~GCObserver() {
    /* TODO: do this before shutting down the JS context
    if (_myContext) {
         JS_SetGCCallback(_myContext, _myPrevCallback);
    }
    */
    AC_TRACE << "DTOR" << endl;
}
#ifdef USE_LEGACY_SPIDERMONKEY
JSBool JS_DLL_CALLBACK
#else
JS_PUBLIC_API(JSBool)
#endif
GCObserver::callbackHook(JSContext *cx, JSGCStatus status) {
    return GCObserver::get().onGarbageCollected(cx, status);
}

void
GCObserver::attach(JSContext * cx) {
    AC_TRACE << "attaching";
     _myContext = cx;
     _myPrevCallback = JS_SetGCCallback(_myContext, callbackHook);
}

#ifdef USE_LEGACY_SPIDERMONKEY
JSBool JS_DLL_CALLBACK
#else
JS_PUBLIC_API(JSBool)
#endif
GCObserver::onGarbageCollected(JSContext *cx, JSGCStatus status) {
    if (status == JSGC_MARK_END) {
        // now go through all the objects in our map and emit the signal if
        // the object is about to be finalized. Then, remove the object from
        // our map.
        for (ObjectMap::iterator i = _myObjectMap.begin(); i != _myObjectMap.end();) {
            JSObject * myWatchedObject = i->first;
            FinalizeSignal & mySignal = i->second;
            if ( JS_IsAboutToBeFinalized(_myContext, myWatchedObject) ) {
                AC_TRACE << "watched JSObject @ " << myWatchedObject << " about to be finalized";
                mySignal.emit(myWatchedObject);
                _myObjectMap.erase(i++); // (see p. 205, Josuttis for erasing while iterating)
            } else {
                ++i;
            }
        }
    }
    if (_myPrevCallback) {
       return _myPrevCallback(cx, status);
    } else {
        return JS_TRUE;
    }
}

GCObserver::FinalizeSignal
GCObserver::watchObject(JSObject * theWatchedObject) {
    AC_TRACE << "adding finalizer watch to JSObject @ " << theWatchedObject;
    pair<ObjectMap::iterator,bool> status = _myObjectMap.insert(
            make_pair(theWatchedObject, FinalizeSignal()));
    return status.first->second;
}

}

