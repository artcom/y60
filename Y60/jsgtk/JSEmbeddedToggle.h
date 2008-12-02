//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef TNT_JS_EMBEDDED_TOGGLE
#define TNT_JS_EMBEDDED_TOGGLE

#include "JSEmbeddedButton.h"

#include <y60/acgtk/EmbeddedToggle.h>

#include <y60/jsbase/JSWrapper.h>

#if defined(_MSC_VER)
#pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm/window.h>
#include <gtkmm/widget.h>
#include <gtkmm/container.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)

#include <asl/base/string_functions.h>

namespace jslib {

class JSEmbeddedToggle : public JSWrapper<acgtk::EmbeddedToggle, asl::Ptr<acgtk::EmbeddedToggle>,
                                      StaticAccessProtocol>
{
        JSEmbeddedToggle();  // hide default constructor
        typedef JSEmbeddedButton JSBASE;
    public:
        virtual ~JSEmbeddedToggle() { 
        }
        typedef acgtk::EmbeddedToggle NATIVE;
        typedef asl::Ptr<acgtk::EmbeddedToggle> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "EmbeddedToggle";
        }

        static JSFunctionSpec * Functions();

        
        enum PropertyNumbers {
            PROP_signal_toggled = JSBASE::PROP_END,
            PROP_active,
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

        JSEmbeddedToggle(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSEmbeddedToggle & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSEmbeddedToggle &>(JSEmbeddedToggle::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSEmbeddedToggle::NATIVE> 
    : public JSClassTraitsWrapper<JSEmbeddedToggle::NATIVE, JSEmbeddedToggle> {};

jsval as_jsval(JSContext *cx, JSEmbeddedToggle::NATIVE * theWindow);
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<acgtk::EmbeddedToggle> & theEmbeddedToggle);

} // namespace

#endif


