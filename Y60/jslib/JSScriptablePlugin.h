//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSScriptablePlugin.h,v $
//   $Author: christian $
//   $Revision: 1.4 $
//   $Date: 2004/10/31 15:58:17 $
//
//
//=============================================================================

#ifndef _ac_jslib_JSScriptablePlugin_h_
#define _ac_jslib_JSScriptablePlugin_h_

#include "IScriptablePlugin.h"

namespace jslib {

    class JSScriptablePlugin {
        public:
            JSScriptablePlugin(IScriptablePluginPtr & theNative) :
                _myNative(theNative)
            {}

            ~JSScriptablePlugin();

            static JSFunctionSpec * Functions();
            static JSPropertySpec * Properties();
            static JSConstIntPropertySpec * ConstIntProperties();
            static JSPropertySpec * StaticProperties();
            static JSFunctionSpec * StaticFunctions();

            virtual unsigned long length() const {
                return 1;
            }

            static JSBool Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
            static void initClass(JSContext *cx, JSObject *theGlobalObject, const char * theClassName);

            static IScriptablePluginPtr & getNative(JSContext *cx, JSObject *obj);
            static JSObject * Construct(JSContext *cx, IScriptablePluginPtr theNative);

        private:
            static JSClass * Class(const char * theClassName);

            IScriptablePluginPtr _myNative;
    };

    jsval as_jsval(JSContext *cx, IScriptablePluginPtr theOwner);
    bool convertFrom(JSContext *cx, jsval theValue, IScriptablePluginPtr & theDest);
}

#endif
