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
//   $RCSfile: JSMenu.h,v $
//   $Author: martin $
//   $Revision: 1.1 $
//   $Date: 2005/03/30 15:54:32 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSMENU_INCLUDED_
#define _Y60_ACGTKSHELL_JSMENU_INCLUDED_

#include "JSMenuShell.h"
#include <y60/JSWrapper.h>
#include <gtkmm/menu.h>

#include <asl/string_functions.h>

namespace jslib {

class JSMenu : public JSWrapper<Gtk::Menu, asl::Ptr<Gtk::Menu>, StaticAccessProtocol> {
    private:
        JSMenu();  // hide default constructor
        typedef JSMenuShell JSBASE;
    public:
        virtual ~JSMenu() {
        }
        typedef Gtk::Menu NATIVE;
        typedef asl::Ptr<Gtk::Menu> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Menu";
        };
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_END = JSBASE::PROP_END
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

        JSMenu(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSMenu & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSMenu &>(JSMenu::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSMenu::NATIVE>
    : public JSClassTraitsWrapper<JSMenu::NATIVE, JSMenu> {};

jsval as_jsval(JSContext *cx, JSMenu::OWNERPTR theOwner, JSMenu::NATIVE * theMenu);

} // namespace

#endif

