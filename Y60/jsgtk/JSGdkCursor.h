//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JS_GDK_CURSOR_INCLUDED_
#define _Y60_ACGTKSHELL_JS_GDK_CURSOR_INCLUDED_

#include <y60/jsbase/JSWrapper.h>
#include <gdkmm/cursor.h>
#include <asl/base/string_functions.h>


namespace jslib {

class JSGdkCursor : public JSWrapper<Gdk::Cursor, asl::Ptr<Gdk::Cursor>, StaticAccessProtocol> {
        JSGdkCursor();  // hide default constructor
    public:
        virtual ~JSGdkCursor() {
        }
        typedef Gdk::Cursor NATIVE;
        typedef asl::Ptr<Gdk::Cursor> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "GdkCursor";
        };
        static JSFunctionSpec * Functions();
        static JSPropertySpec * Properties();

        static JSPropertySpec * StaticProperties();
        static JSFunctionSpec * StaticFunctions();

        virtual unsigned long length() const {
            return 1;
        }
        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSGdkCursor(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
       //  static void addClassProperties(JSContext * cx, JSObject * theClassProto);
        // getproperty handling
        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSGdkCursor & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSGdkCursor &>(JSGdkCursor::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSGdkCursor::NATIVE>
    : public JSClassTraitsWrapper<JSGdkCursor::NATIVE, JSGdkCursor> {};

jsval as_jsval(JSContext *cx, JSGdkCursor::OWNERPTR theOwner, JSGdkCursor::NATIVE * theNative);

bool convertFrom(JSContext *cx, jsval theValue, JSGdkCursor::NATIVE * & theCursor);

} // namespace

#endif


