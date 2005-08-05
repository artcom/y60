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
//   $RCSfile: JSMenuItem.h,v $
//   $Author: david $
//   $Revision: 1.3 $
//   $Date: 2005/04/13 15:24:13 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSMENUITEM_INCLUDED_
#define _Y60_ACGTKSHELL_JSMENUITEM_INCLUDED_

#include "JSBin.h"
#include <y60/JSWrapper.h>
#include <gtkmm/menuitem.h>

#include <asl/string_functions.h>

namespace jslib {

class JSMenuItem : public JSWrapper<Gtk::MenuItem, asl::Ptr<Gtk::MenuItem>, StaticAccessProtocol> {
        JSMenuItem();  // hide default constructor
    private:
        typedef JSBin JSBASE;
    public:
        virtual ~JSMenuItem() {
        }
        typedef Gtk::MenuItem NATIVE;
        typedef asl::Ptr<Gtk::MenuItem> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "MenuItem";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_signal_activate = JSBASE::PROP_END,
            PROP_signal_activate_item,
            PROP_submenu,
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

        JSMenuItem(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative) {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSMenuItem & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSMenuItem &>(JSMenuItem::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSMenuItem::NATIVE>
    : public JSClassTraitsWrapper<JSMenuItem::NATIVE, JSMenuItem> {};

jsval as_jsval(JSContext *cx, JSMenuItem::OWNERPTR theOwner, JSMenuItem::NATIVE * theNative);

} // namespace

#endif

