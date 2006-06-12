//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


#include "JSgtkutils.h"

#include <y60/QuitFlagSingleton.h>
#include <asl/Logger.h>

namespace jslib {

JSBool
JSA_CallFunctionName(JSContext * cx, JSObject * obj, const Glib::ustring & theName, int argc, jsval argv[], jsval* rval) {

    JSBool bOK = JS_TRUE;
    gunichar2 * myUTF16 = 0;
    try {
        try {
            glong myLength = g_utf8_strlen(theName.c_str(), -1);
            myUTF16 = g_utf8_to_utf16(theName.c_str(), -1,0,0,0);
            jsval myVal;
            bOK = JS_GetUCProperty(cx, obj, reinterpret_cast<jschar*>(myUTF16), myLength, &myVal);
            if (myVal == JSVAL_VOID) {
                AC_WARNING << "no JS function named '" << theName << "' found.";
            } else { 
                bOK = JS_CallFunctionValue(cx, obj, myVal, argc, argv, rval);
                if (!bOK && !QuitFlagSingleton::get().getQuitFlag()) {
                    AC_ERROR << "Exception while calling js function '" << theName << "'";
                    JSA_reportUncaughtException(cx, cx->errorReporter);
                }
            }
            g_free(myUTF16);
        } catch (...) {
            if (myUTF16) {
                g_free(myUTF16);
            }
            throw;
        }
    } HANDLE_CPP_EXCEPTION; 
    return bOK;
}

}

