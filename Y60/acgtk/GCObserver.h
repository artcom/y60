//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_jslib_gcobserver_included_
#define _ac_y60_jslib_gcobserver_included_

#include <y60/jssettings.h>
#include <asl/Singleton.h>

#include <js/jsapi.h>
#include <sigc++/sigc++.h>

#include <map>

namespace jslib {
    

class GCObserver : public asl::Singleton<GCObserver> {
    public:
        typedef sigc::signal<void, JSObject*> FinalizeSignal;

        GCObserver();
        virtual ~GCObserver();
        void attach(JSContext * cx);
        FinalizeSignal watchObject(JSObject * theWatchedObject);
    private:
        typedef std::map<JSObject*, FinalizeSignal> ObjectMap;
        JSContext * _myContext;
        JSGCCallback _myPrevCallback;
        ObjectMap _myObjectMap;

        static JSBool JS_DLL_CALLBACK callbackHook(JSContext *cx, JSGCStatus status);
        JSBool JS_DLL_CALLBACK onGarbageCollected(JSContext *cx, JSGCStatus status);
};

}

#endif
