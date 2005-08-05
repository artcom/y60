//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSRuler.h,v $
//   $Author: martin $
//   $Revision: 1.2 $
//   $Date: 2005/04/12 12:21:48 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSRULER_INCLUDED_
#define _Y60_ACGTKSHELL_JSRULER_INCLUDED_

#include "JSWidget.h"
#include <y60/JSWrapper.h>
#include <gtkmm/ruler.h>

#include <asl/string_functions.h>

namespace jslib {

class JSRuler : public JSWrapper<Gtk::Ruler, asl::Ptr<Gtk::Ruler>, StaticAccessProtocol> {
    private:
        JSRuler();  // hide default constructor
        typedef JSWidget JSBASE;
    public:
        virtual ~JSRuler() {
        }
        typedef Gtk::Ruler NATIVE;
        typedef asl::Ptr<Gtk::Ruler> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Ruler";
        };
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_metric = JSBASE::PROP_END,
            PROP_upper,
            PROP_lower,
            PROP_position,
            PROP_max_size,
            PROP_CENTIMETERS,
            PROP_INCHES,
            PROP_PIXELS,
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

        JSRuler(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSRuler & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSRuler &>(JSRuler::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSRuler::NATIVE>
    : public JSClassTraitsWrapper<JSRuler::NATIVE, JSRuler> {};

jsval as_jsval(JSContext *cx, JSRuler::OWNERPTR theOwner, JSRuler::NATIVE * theRuler);

} // namespace

#endif

