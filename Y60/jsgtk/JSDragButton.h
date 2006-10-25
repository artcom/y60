//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef TNT_JS_DRAG_BUTTON
#define TNT_JS_DRAG_BUTTON

#include "JSEmbeddedButton.h"

#include <acgtk/DragButton.h>

#include <y60/JSWrapper.h>
#include <gtkmm/window.h>
#include <gtkmm/widget.h>
#include <gtkmm/container.h>

#include <asl/string_functions.h>

namespace jslib {

class JSDragButton : public JSWrapper<acgtk::DragButton, asl::Ptr<acgtk::DragButton>,
                                      StaticAccessProtocol>
{
        JSDragButton();  // hide default constructor
        typedef JSEmbeddedButton JSBASE;
    public:
        virtual ~JSDragButton() { 
        }
        typedef acgtk::DragButton NATIVE;
        typedef asl::Ptr<acgtk::DragButton> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "DragButton";
        }

        static JSFunctionSpec * Functions();

        
        enum PropertyNumbers {
            PROP_signal_drag_start = JSBASE::PROP_END,
            PROP_signal_drag,
            PROP_signal_drag_done,
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

        JSDragButton(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSDragButton & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSDragButton &>(JSDragButton::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSDragButton::NATIVE> 
    : public JSClassTraitsWrapper<JSDragButton::NATIVE, JSDragButton> {};

jsval as_jsval(JSContext *cx, JSDragButton::NATIVE * theWindow);
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<acgtk::DragButton> & theDragButton);

} // namespace

#endif // TNT_JS_VIEW_PANE


