//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


#ifndef _Y60_ACGTKSHELL_JSBUTTONBOX_INCLUDED_
#define _Y60_ACGTKSHELL_JSBUTTONBOX_INCLUDED_

#include "JSBox.h"
#include <y60/JSWrapper.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/container.h>

#include <asl/string_functions.h>

namespace jslib {

class JSButtonBox : public JSWrapper<Gtk::ButtonBox, asl::Ptr<Gtk::ButtonBox>, StaticAccessProtocol> {
    private:
        JSButtonBox();  // hide default constructor
        typedef JSBox JSBASE;
    public:
        virtual ~JSButtonBox() {
        }
        typedef Gtk::ButtonBox NATIVE;
        typedef asl::Ptr<Gtk::ButtonBox> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "ButtonBox";
        };
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_homogeneous = JSBASE::PROP_END,
            PROP_spacing,
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

        JSButtonBox(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSButtonBox & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSButtonBox &>(JSButtonBox::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSButtonBox::NATIVE>
    : public JSClassTraitsWrapper<JSButtonBox::NATIVE, JSButtonBox> {};

jsval as_jsval(JSContext *cx, JSButtonBox::OWNERPTR theOwner, JSButtonBox::NATIVE * theBox);

} // namespace

#endif

