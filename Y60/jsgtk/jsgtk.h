//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: jsgtk.h,v $
//   $Author: christian $
//   $Revision: 1.23 $
//   $Date: 2005/04/21 17:58:32 $
//
//
//=============================================================================

#ifndef __Y60_ACXPSHELL_GTKCPP_INCLUDED__
#define __Y60_ACXPSHELL_GTKCPP_INCLUDED__

#include <y60/jssettings.h>
#include <js/jsapi.h>
#include <gtkmm.h>

extern bool initGtkClasses(JSContext *cx, JSObject *theGlobalObject);

namespace jslib {


bool convertFrom(JSContext *cx, jsval theValue, Glib::ustring & theTarget);

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
