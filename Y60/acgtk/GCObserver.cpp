//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "GCObserver.h"
#include <asl/Logger.h>

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
JSBool JS_DLL_CALLBACK 
GCObserver::callbackHook(JSContext *cx, JSGCStatus status) {
    return GCObserver::get().onGarbageCollected(cx, status);
}
    
void
GCObserver::attach(JSContext * cx) {
    AC_TRACE << "attaching";
     _myContext = cx;
     _myPrevCallback = JS_SetGCCallback(_myContext, callbackHook);
}

JSBool JS_DLL_CALLBACK 
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

