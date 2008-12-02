//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef __Y60_JSGTK_JSGTKUTILS_INCLUDED__
#define __Y60_JSGTK_JSGTKUTILS_INCLUDED__

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/jssettings.h>
#include <js/spidermonkey/jsapi.h>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4244 4512 4413 4251 4127 4520)
#endif //defined(_MSC_VER)
#include <gtkmm.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

namespace jslib {

JSBool
JSA_CallFunctionName(JSContext * cx, JSObject * obj, const Glib::ustring & theName, int argc, jsval argv[], jsval* rval);


inline
jsval as_jsval(JSContext *cx, const Glib::ustring & theUTF8String) {
    return as_jsval(cx, theUTF8String.data());
}

inline
bool convertFrom(JSContext *cx, jsval theValue, Glib::ustring & theDest) {
    theDest = asl::as_string(cx, theValue);
    return true;
}
    
} // namespace
#endif

