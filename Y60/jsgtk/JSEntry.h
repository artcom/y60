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
//   $RCSfile: JSEntry.h,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/11/27 16:22:00 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSVENTRY_INCLUDED_
#define _Y60_ACGTKSHELL_JSVENTRY_INCLUDED_

#include "JSWidget.h"
#include <y60/JSWrapper.h>
#include <gtkmm/entry.h>

#include <asl/string_functions.h>

namespace jslib {

class JSEntry : public JSWrapper<Gtk::Entry, asl::Ptr<Gtk::Entry>, StaticAccessProtocol> {
        JSEntry();  // hide default constructor
    typedef JSWidget JSBASE;
    public:
        virtual ~JSEntry() {
        }
        typedef Gtk::Entry NATIVE;
        typedef asl::Ptr<Gtk::Entry> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Entry";
        };
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_visibility = JSBASE::PROP_END,
            PROP_has_frame,
            PROP_max_length,
            PROP_activates_default,
            PROP_width_chars,
            PROP_text,
            PROP_signal_changed,
            PROP_signal_editing_done,
            PROP_END
        };
        static JSPropertySpec * Properties();

        virtual unsigned long length() const {
            return 1;
        }

        static JSBool getPropertySwitch(NATIVE & theNative, unsigned long theID,
                JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        static JSBool setPropertySwitch(NATIVE & theNative, unsigned long theID, JSContext *cx,
                JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSEntry(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSEntry & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSEntry &>(JSEntry::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSEntry::NATIVE>
    : public JSClassTraitsWrapper<JSEntry::NATIVE, JSEntry> {};

jsval as_jsval(JSContext *cx, JSEntry::OWNERPTR theOwner, JSEntry::NATIVE * theEntry);

} // namespace

#endif


