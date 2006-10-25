//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSSTATUSBAR_INCLUDED_
#define _Y60_ACGTKSHELL_JSSTATUSBAR_INCLUDED_

#include "JSHBox.h"
#include <y60/JSWrapper.h>
#include <gtkmm/statusbar.h>

#include <asl/string_functions.h>

namespace jslib {

class JSStatusBar : public JSWrapper<Gtk::Statusbar, asl::Ptr<Gtk::Statusbar>, StaticAccessProtocol> {
    private:
        JSStatusBar();  // hide default constructor
        typedef JSHBox JSBASE;
    public:
        virtual ~JSStatusBar() {
        }
        typedef Gtk::Statusbar NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Statusbar";
        };
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_has_resize_grip = JSBASE::PROP_END,
            PROP_signal_text_pushed,
            PROP_signal_text_popped,
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

        JSStatusBar(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSStatusBar & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSStatusBar &>(JSStatusBar::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSStatusBar::NATIVE>
    : public JSClassTraitsWrapper<JSStatusBar::NATIVE, JSStatusBar> {};

jsval as_jsval(JSContext *cx, JSStatusBar::OWNERPTR theOwner, JSStatusBar::NATIVE * theStatusBar);

} // namespace

#endif

