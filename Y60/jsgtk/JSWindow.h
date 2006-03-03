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
//   $RCSfile: JSWindow.h,v $
//   $Author: christian $
//   $Revision: 1.8 $
//   $Date: 2005/04/24 17:54:19 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSWINDOW_INCLUDED_
#define _Y60_ACGTKSHELL_JSWINDOW_INCLUDED_

#include "JSBin.h"
#include <y60/JSWrapper.h>
#include <gtkmm/window.h>
#include <gtkmm/container.h>

#include <asl/string_functions.h>

namespace jslib {

class JSWindow : public JSWrapper<Gtk::Window, asl::Ptr<Gtk::Window>, StaticAccessProtocol> {
    private:
        JSWindow();  // hide default constructor
        typedef JSBin JSBASE;
    public:
        typedef Gtk::Window NATIVE;
        typedef asl::Ptr<Gtk::Window> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Window";
        };
        static std::string FullClassName();
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_title = JSBASE::PROP_END,
            PROP_decorated,
            PROP_position,
            PROP_size,
            PROP_modal,
            PROP_screenSize,
            PROP_type_hint,
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

        JSWindow(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {
        }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSWindow & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSWindow &>(JSWindow::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSWindow::NATIVE>
    : public JSClassTraitsWrapper<JSWindow::NATIVE, JSWindow> {};

jsval as_jsval(JSContext *cx, JSWindow::OWNERPTR theOwner, JSWindow::NATIVE * theNative);

} // namespace

#endif

