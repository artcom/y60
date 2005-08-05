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


#ifndef _ac_jsgtk_JSSeparator_h_
#define _ac_jsgtk_JSSeparator_h_

#include "JSWidget.h"
#include <y60/JSWrapper.h>
#include <gtkmm/separator.h>
#include <gtkmm/widget.h>

#include <asl/string_functions.h>

namespace jslib {
/**
 * 
 * @ingroup Y60jsgtk
 */ 
class JSSeparator : public JSWrapper<Gtk::Separator, asl::Ptr<Gtk::Separator>, StaticAccessProtocol> {
    private:
        JSSeparator();  // hide default constructor
        typedef JSWidget JSBASE;
    public:
        virtual ~JSSeparator() {
        }
        typedef Gtk::Separator NATIVE;
        typedef asl::Ptr<Gtk::Separator> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Separator";
        };

        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
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

        JSSeparator(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSSeparator & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSSeparator &>(JSSeparator::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSSeparator::NATIVE>
    : public JSClassTraitsWrapper<JSSeparator::NATIVE, JSSeparator> {};

jsval as_jsval(JSContext *cx, JSSeparator::OWNERPTR theOwner, JSSeparator::NATIVE * theSeparator);


}

#endif // _ac_jsgtk_JSSeparator_h_
