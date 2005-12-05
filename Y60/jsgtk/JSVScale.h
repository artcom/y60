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
//   $RCSfile: JSVScale.h,v $
//   $Author: david $
//   $Revision: 1.1 $
//   $Date: 2005/03/03 17:28:53 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSVSCALE_INCLUDED_
#define _Y60_ACGTKSHELL_JSVSCALE_INCLUDED_

#include "JSScale.h"

#include <y60/JSWrapper.h>
#include <gtkmm/scale.h>

#include <asl/string_functions.h>

namespace jslib {

class JSVScale : public JSWrapper<Gtk::VScale, asl::Ptr<Gtk::VScale>, StaticAccessProtocol> {
    private:
        JSVScale();  // hide default constructor
        typedef JSScale JSBASE;
    public:
        virtual ~JSVScale() {
        }
        typedef Gtk::VScale NATIVE;
        typedef asl::Ptr<Gtk::VScale> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "VScale";
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

        JSVScale(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSVScale & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSVScale &>(JSVScale::getJSWrapper(cx,obj));
        }

    private:
};


template <>
struct JSClassTraits<JSVScale::NATIVE>
    : public JSClassTraitsWrapper<JSVScale::NATIVE, JSVScale> {};

jsval as_jsval(JSContext *cx, JSVScale::OWNERPTR theOwner, JSVScale::NATIVE * theVScale);

} // namespace

#endif

