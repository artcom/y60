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
//   $RCSfile: JSColorButton.h,v $
//   $Author: david $
//   $Revision: 1.1 $
//   $Date: 2005/01/31 09:42:07 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSCOLORBUTTON_INCLUDED_
#define _Y60_ACGTKSHELL_JSCOLORBUTTON_INCLUDED_

#include "JSButton.h"
#include <y60/JSWrapper.h>
#include <gtkmm/colorbutton.h>

#include <asl/string_functions.h>

namespace jslib {

class JSColorButton : public JSWrapper<Gtk::ColorButton, asl::Ptr<Gtk::ColorButton>, StaticAccessProtocol> {
        JSColorButton();  // hide default constructor
        typedef JSButton JSBASE;
    public:
        virtual ~JSColorButton() {
        }
        typedef Gtk::ColorButton NATIVE;
        typedef asl::Ptr<Gtk::ColorButton> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "ColorButton";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
             PROP_color = JSBASE::PROP_END,
             PROP_alpha,
             PROP_use_alpha,
             PROP_title,
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

        JSColorButton(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSColorButton & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSColorButton &>(JSColorButton::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSColorButton::NATIVE>
    : public JSClassTraitsWrapper<JSColorButton::NATIVE, JSColorButton> {};

jsval as_jsval(JSContext *cx, JSColorButton::OWNERPTR theOwner, JSColorButton::NATIVE * theColorButton);

} // namespace

#endif

