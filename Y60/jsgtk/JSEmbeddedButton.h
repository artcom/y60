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
//   $RCSfile: JSEmbeddedButton.h,v $
//   $Author: martin $
//   $Revision: 1.6 $
//   $Date: 2005/03/30 15:54:32 $
//
//
//=============================================================================

#ifndef TNT_JS_EMBEDDED_BUTTON
#define TNT_JS_EMBEDDED_BUTTON

#include "JSEventBox.h"

#include <acgtk/EmbeddedButton.h>

#include <y60/JSWrapper.h>
#include <gtkmm/window.h>
#include <gtkmm/widget.h>
#include <gtkmm/container.h>

#include <asl/string_functions.h>

namespace jslib {

class JSEmbeddedButton : public JSWrapper<acgtk::EmbeddedButton, asl::Ptr<acgtk::EmbeddedButton>,
                                      StaticAccessProtocol>
{
        JSEmbeddedButton();  // hide default constructor
        typedef JSEventBox JSBASE;
    public:
        virtual ~JSEmbeddedButton() { 
        }
        typedef acgtk::EmbeddedButton NATIVE;
        typedef asl::Ptr<acgtk::EmbeddedButton> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "EmbeddedButton";
        }

        static JSFunctionSpec * Functions();

        
        enum PropertyNumbers {
            PROP_signal_clicked = JSBASE::PROP_END,
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

        JSEmbeddedButton(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSEmbeddedButton & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSEmbeddedButton &>(JSEmbeddedButton::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSEmbeddedButton::NATIVE> 
    : public JSClassTraitsWrapper<JSEmbeddedButton::NATIVE, JSEmbeddedButton> {};

jsval as_jsval(JSContext *cx, JSEmbeddedButton::NATIVE * theWindow);
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<acgtk::EmbeddedButton> & theEmbeddedButton);

} // namespace

#endif  


