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

#include <y60/jssettings.h>
#include <js/jsapi.h>
#include <gtkmm.h>

namespace jslib {

// convert from glib's utf8 to JS utf-16
// jsval as_jsval(JSContext *cx, const Glib::ustring & theUTF8String);


// convert from JS utf-16 to glib's utf-8
// bool convertFrom(JSContext *cx, jsval theValue, Glib::ustring & theTarget);


}
    
#endif

