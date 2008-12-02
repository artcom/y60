//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef __Y60_ACXPSHELL_GTKCPP_INCLUDED__
#define __Y60_ACXPSHELL_GTKCPP_INCLUDED__

#include "JSgtkutils.h"

#include <y60/jsbase/jssettings.h>
#include <js/spidermonkey/jsapi.h>
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4512 4413)
#endif //defined(_MSC_VER)
#include <gtkmm.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

extern bool initGtkClasses(JSContext *cx, JSObject *theGlobalObject);

namespace jslib {

template<class TARGET>
class ConvertFrom {
    public:
        static bool convert(JSContext *cx, jsval theValue, Glib::RefPtr<TARGET> & theTarget);
        static bool convert(JSContext *cx, jsval theValue, TARGET *& theTarget);
};

template<class TARGET>
bool convertFrom(JSContext *cx, jsval theValue, TARGET *& theTarget) {
    return ConvertFrom<TARGET>::convert(cx, theValue, theTarget);
}

template<class TARGET>
bool convertFrom(JSContext *cx, jsval theValue, Glib::RefPtr<TARGET> & theTarget) {
    return ConvertFrom<TARGET>::convert(cx, theValue, theTarget);
}

jsval gtk_jsval(JSContext *cx, Gtk::Widget * theWidget, bool takeOwnership = false);


} // namespace

#endif
