//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSBOX_INCLUDED_
#define _Y60_ACGTKSHELL_JSBOX_INCLUDED_

#include "JSContainer.h"
#include <y60/jsbase/JSWrapper.h>

#if defined(_MSC_VER)
#pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm/box.h>
#include <gtkmm/container.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#include <asl/base/string_functions.h>

namespace jslib {

class JSBox : public JSWrapper<Gtk::Box, asl::Ptr<Gtk::Box>, StaticAccessProtocol> {
    private:
        JSBox();  // hide default constructor
        typedef JSContainer JSBASE;
    public:
        virtual ~JSBox() {
        }
        typedef Gtk::Box NATIVE;
        typedef asl::Ptr<Gtk::Box> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Box";
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

        JSBox(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSBox & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSBox &>(JSBox::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSBox::NATIVE>
    : public JSClassTraitsWrapper<JSBox::NATIVE, JSBox> {};

jsval as_jsval(JSContext *cx, JSBox::OWNERPTR theOwner, JSBox::NATIVE * theBox);

} // namespace

#endif

