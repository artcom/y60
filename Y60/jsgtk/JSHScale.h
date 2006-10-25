//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSHSCALE_INCLUDED_
#define _Y60_ACGTKSHELL_JSHSCALE_INCLUDED_

#include "JSScale.h"

#include <y60/JSWrapper.h>
#include <gtkmm/scale.h>

#include <asl/string_functions.h>

namespace jslib {

class JSHScale : public JSWrapper<Gtk::HScale, asl::Ptr<Gtk::HScale>, StaticAccessProtocol> {
    private:
        JSHScale();  // hide default constructor
        typedef JSScale JSBASE;
    public:
        virtual ~JSHScale() {
        }
        typedef Gtk::HScale NATIVE;
        typedef asl::Ptr<Gtk::HScale> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "HScale";
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

        JSHScale(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSHScale & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSHScale &>(JSHScale::getJSWrapper(cx,obj));
        }

    private:
};


template <>
struct JSClassTraits<JSHScale::NATIVE>
    : public JSClassTraitsWrapper<JSHScale::NATIVE, JSHScale> {};

jsval as_jsval(JSContext *cx, JSHScale::OWNERPTR theOwner, JSHScale::NATIVE * theHScale);

} // namespace

#endif

