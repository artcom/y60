//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSHPANED_INCLUDED_
#define _Y60_ACGTKSHELL_JSHPANED_INCLUDED_

#include "JSPaned.h"

#include <y60/JSWrapper.h>
#include <gtkmm/paned.h>

#include <asl/string_functions.h>

namespace jslib {

class JSHPaned : public JSWrapper<Gtk::HPaned, asl::Ptr<Gtk::HPaned>, StaticAccessProtocol> {
        JSHPaned();  // hide default constructor
        typedef JSPaned JSBASE;
    public:
        virtual ~JSHPaned() {
        }
        typedef Gtk::HPaned NATIVE;
        typedef asl::Ptr<Gtk::HPaned> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "HPaned";
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

        JSHPaned(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSHPaned & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSHPaned &>(JSHPaned::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSHPaned::NATIVE>
    : public JSClassTraitsWrapper<JSHPaned::NATIVE, JSHPaned> {};

jsval as_jsval(JSContext *cx, JSHPaned::OWNERPTR theOwner, JSHPaned::NATIVE * theBin);

} // namespace

#endif



