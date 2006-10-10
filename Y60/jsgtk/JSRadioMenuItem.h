//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSRADIOMENUITEM_INCLUDED_
#define _Y60_ACGTKSHELL_JSRADIOMENUITEM_INCLUDED_

#include "JSMenuItem.h"
#include <y60/JSWrapper.h>
#include <gtkmm/radiomenuitem.h>

#include <asl/string_functions.h>

namespace jslib {

class JSRadioMenuItem : public JSWrapper<Gtk::RadioMenuItem, asl::Ptr<Gtk::RadioMenuItem>, StaticAccessProtocol> {
        JSRadioMenuItem();  // hide default constructor
    private:
        typedef JSMenuItem JSBASE;
    public:
        virtual ~JSRadioMenuItem() {
        }
        typedef Gtk::RadioMenuItem NATIVE;
        typedef asl::Ptr<Gtk::RadioMenuItem> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "RadioMenuItem";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_active = JSBASE::PROP_END
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

        JSRadioMenuItem(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative) {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSRadioMenuItem & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSRadioMenuItem &>(JSRadioMenuItem::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSRadioMenuItem::NATIVE>
    : public JSClassTraitsWrapper<JSRadioMenuItem::NATIVE, JSRadioMenuItem> {};

jsval as_jsval(JSContext *cx, JSRadioMenuItem::OWNERPTR theOwner, JSRadioMenuItem::NATIVE * theNative);

} // namespace

#endif

