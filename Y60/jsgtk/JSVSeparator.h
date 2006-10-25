//============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_jsgtk_JSVSeparator_h_
#define _ac_jsgtk_JSVSeparator_h_

#include "JSSeparator.h"
#include <y60/JSWrapper.h>
#include <gtkmm/separator.h>

#include <asl/string_functions.h>

namespace jslib {
/**
 * 
 * @ingroup Y60jsgtk
 */ 
class JSVSeparator : public JSWrapper<Gtk::VSeparator, asl::Ptr<Gtk::VSeparator>, StaticAccessProtocol> {
    private:
        JSVSeparator();  // hide default constructor
        typedef JSSeparator JSBASE;
    public:
        virtual ~JSVSeparator() {
        }
        typedef Gtk::VSeparator NATIVE;
        typedef asl::Ptr<Gtk::VSeparator> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "VSeparator";
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

        JSVSeparator(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        { }

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSVSeparator & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSVSeparator &>(JSVSeparator::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSVSeparator::NATIVE>
    : public JSClassTraitsWrapper<JSVSeparator::NATIVE, JSVSeparator> {};

jsval as_jsval(JSContext *cx, JSVSeparator::OWNERPTR theOwner, JSVSeparator::NATIVE * theVSeparator);

} // namespace


#endif // _ac_jsgtk_JSVSeparator_h_
