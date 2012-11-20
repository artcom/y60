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

#ifndef _ac_jslib_JSScriptablePlugin_h_
#define _ac_jslib_JSScriptablePlugin_h_

#include "y60_jsbase_settings.h"

#include "IScriptablePlugin.h"

namespace jslib {

    class Y60_JSBASE_DECL JSScriptablePlugin {
        public:
            JSScriptablePlugin(IScriptablePluginPtr & theNative) :
                _myNative(theNative)
            {}

            virtual ~JSScriptablePlugin();

            static JSFunctionSpec * Functions();
            static JSPropertySpec * Properties();
            static JSConstIntPropertySpec * ConstIntProperties();
            static JSPropertySpec * StaticProperties();
            static JSFunctionSpec * StaticFunctions();

            virtual unsigned long length() const {
                return 1;
            }

            static JSBool Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
            static void initClass(JSContext *cx, JSObject *theGlobalObject, const char * theClassName,
                    JSFunctionSpec * theFunctions = 0, JSFunctionSpec * theStaticFunctions = 0,
                    JSConstIntPropertySpec * theConstIntProperties = 0, JSPropertySpec * theStaticProperties = 0);

            static IScriptablePluginPtr & getNative(JSContext *cx, JSObject *obj);
            static JSObject * Construct(JSContext *cx, IScriptablePluginPtr theNative);

        private:

            static JSBool getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
            static JSBool setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
            static bool isFunction( JSContext * cx, JSObject * obj, const std::string & theProperty );

            static std::vector<JSFunctionSpec> mergeFunctions( JSFunctionSpec * theFunctions, JSFunctionSpec * theOtherFunctions);

            static JSClass * Class(const char * theClassName);

            IScriptablePluginPtr _myNative;
    };

    Y60_JSBASE_DECL jsval as_jsval(JSContext *cx, IScriptablePluginPtr theOwner);
    Y60_JSBASE_DECL bool convertFrom(JSContext *cx, jsval theValue, IScriptablePluginPtr & theDest);

    template <class T>
    asl::Ptr<T> getNativeAs(JSContext *cx, JSObject *obj) {
            return dynamic_cast_Ptr<T>(JSScriptablePlugin::getNative(cx, obj));
    }

}

#endif
