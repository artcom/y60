//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSSCROLLEDWINDOW_INCLUDED_
#define _Y60_ACGTKSHELL_JSSCROLLEDWINDOW_INCLUDED_

#include "JSBin.h"
#include <y60/jsbase/JSWrapper.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/container.h>

#include <asl/base/string_functions.h>

namespace jslib {

class JSScrolledWindow : public JSWrapper<Gtk::ScrolledWindow, asl::Ptr<Gtk::ScrolledWindow>, StaticAccessProtocol> {
    private:
        JSScrolledWindow();  // hide default constructor
        typedef JSBin JSBASE;
    public:
        typedef Gtk::ScrolledWindow NATIVE;
        typedef asl::Ptr<Gtk::ScrolledWindow> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "ScrolledWindow";
        };
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_END = JSBASE::PROP_END,
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

        JSScrolledWindow(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {
        }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSScrolledWindow & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSScrolledWindow &>(JSScrolledWindow::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSScrolledWindow::NATIVE>
    : public JSClassTraitsWrapper<JSScrolledWindow::NATIVE, JSScrolledWindow> {};

jsval as_jsval(JSContext *cx, JSScrolledWindow::OWNERPTR theOwner, JSScrolledWindow::NATIVE * theNative);

} // namespace

#endif

