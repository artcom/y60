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
//   $RCSfile: JSTooltips.h,v $
//   $Author: martin $
//   $Revision: 1.4 $
//   $Date: 2004/11/27 16:22:00 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSTOOLTIPS_INCLUDED_
#define _Y60_ACGTKSHELL_JSTOOLTIPS_INCLUDED_

#include <y60/JSWrapper.h>
#include <gtkmm/tooltips.h>

#include <asl/string_functions.h>

namespace jslib {

class JSTooltips : public JSWrapper<Gtk::Tooltips, asl::Ptr<Gtk::Tooltips>, StaticAccessProtocol> {
        JSTooltips();  // hide default constructor
    public:
        virtual ~JSTooltips() {
        }
        typedef Gtk::Tooltips NATIVE;
        typedef asl::Ptr<Gtk::Tooltips> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Tooltips";
        }

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_END = -127
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

        JSTooltips(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        // static JSConstIntPropertySpec * ConstIntProperties();
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSTooltips & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSTooltips &>(JSTooltips::getJSWrapper(cx,obj));
        }
    private:

};

template <>
struct JSClassTraits<JSTooltips::NATIVE>
    : public JSClassTraitsWrapper<JSTooltips::NATIVE, JSTooltips> {};

jsval as_jsval(JSContext *cx, JSTooltips::OWNERPTR theOwner, JSTooltips::NATIVE * theTooltips);

bool convertFrom(JSContext *cx, jsval theValue, JSTooltips::NATIVE *& theTarget);

}

#endif



