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
*/

#ifndef _ac_y60_jslib_gcobserver_included_
#define _ac_y60_jslib_gcobserver_included_

#include "y60_acgtk_settings.h"

#include <y60/jsbase/jssettings.h>
#include <asl/base/Singleton.h>

#ifdef USE_LEGACY_SPIDERMONKEY
#include <js/spidermonkey/jsapi.h>
#else
#include <js/jsapi.h>
#endif

#include <sigc++/sigc++.h>

#include <map>

namespace jslib {


class Y60_ACGTK_DECL GCObserver : public asl::Singleton<GCObserver> {
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
#ifdef USE_LEGACY_SPIDERMONKEY
        static JSBool JS_DLL_CALLBACK callbackHook(JSContext *cx, JSGCStatus status);
        JSBool JS_DLL_CALLBACK onGarbageCollected(JSContext *cx, JSGCStatus status);
#else
        static  JS_PUBLIC_API(JSBool) callbackHook(JSContext *cx, JSGCStatus status);
       JS_PUBLIC_API(JSBool) onGarbageCollected(JSContext *cx, JSGCStatus status);
#endif
};

}

#endif
