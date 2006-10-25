//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSGTK_ENUMS_INCLUDED_
#define _Y60_ACGTKSHELL_JSGTK_ENUMS_INCLUDED_

#include <y60/JSWrapper.h>
#include <gtkmm/enums.h>

#include <asl/string_functions.h>

namespace jslib {

class JSGtk : public JSWrapper<bool, asl::Ptr<bool>, StaticAccessProtocol> {
        JSGtk();  // hide default constructor
    public:
        virtual ~JSGtk() {
        }
        typedef bool NATIVE;
        typedef asl::Ptr<bool> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Gtk";
        };
        static JSFunctionSpec * Functions();

        static JSPropertySpec * Properties();

        virtual unsigned long length() const {
            return 1;
        }
        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSGtk(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSGtk & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSGtk &>(JSGtk::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSGtk::NATIVE>
    : public JSClassTraitsWrapper<JSGtk::NATIVE, JSGtk> {};

jsval as_jsval(JSContext *cx, JSGtk::OWNERPTR theOwner, JSGtk::NATIVE * theBin);

} // namespace

#endif



