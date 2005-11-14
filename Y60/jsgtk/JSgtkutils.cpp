//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


#include "JSgtkutils.h"

#include <asl/Logger.h>

namespace jslib {

jsval as_jsval(JSContext *cx, const Glib::ustring & theUTF8String) {
    gunichar2 * myUTF16 = g_utf8_to_utf16(theUTF8String.data(), theUTF8String.bytes(),0,0,0);
    
    JSString * myString = JS_NewUCStringCopyZ(cx,reinterpret_cast<jschar*>(myUTF16));
    g_free(myUTF16);

    return STRING_TO_JSVAL(myString);
}

bool convertFrom(JSContext *cx, jsval theValue, Glib::ustring & theDest) {
    JSString *myJSStr = JS_ValueToString(cx, theValue);
    if (!myJSStr) {
        return false;
    }
    size_t srcLen = JS_GetStringLength(myJSStr);
  
    // get pointer to 16-bit chars
    gunichar2 * myData = reinterpret_cast<gunichar2*>(JS_GetStringChars(myJSStr));

    // now convert to utf-8 encoded c-string
    glong targetLen;
    gchar * myUTF8 = g_utf16_to_utf8(myData, srcLen * sizeof(gunichar2), 0, &targetLen, 0); 

    // now convert to ustring
    theDest = Glib::ustring(myUTF8);

    // clean up
    g_free(myUTF8);
    return true;
};


}

