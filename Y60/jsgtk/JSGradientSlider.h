//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef TNT_JS_GRADIENT_SLIDER_INCLUDED
#define TNT_JS_GRADIENT_SLIDER_INCLUDED

#include "JSDrawingArea.h"

#include <acgtk/GradientSlider.h>

#include <y60/JSWrapper.h>
#include <gtkmm/window.h>
#include <gtkmm/widget.h>
#include <gtkmm/container.h>

#include <asl/string_functions.h>

namespace jslib {

class JSGradientSlider : public JSWrapper<acgtk::GradientSlider, asl::Ptr<acgtk::GradientSlider>,
                                      StaticAccessProtocol>
{
        JSGradientSlider();  // hide default constructor
        typedef JSDrawingArea JSBASE;
    public:
        virtual ~JSGradientSlider() { 
        }
        typedef acgtk::GradientSlider NATIVE;
        typedef asl::Ptr<acgtk::GradientSlider> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "GradientSlider";
        }

        static JSFunctionSpec * Functions();

        
        enum PropertyNumbers {
            PROP_value_range = JSBASE::PROP_END,
            PROP_transfer_function,
            PROP_selected_color,
            PROP_signalValuesChanged,
            PROP_signalSelectionChanged,
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

        JSGradientSlider(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSGradientSlider & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSGradientSlider &>(JSGradientSlider::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSGradientSlider::NATIVE> 
    : public JSClassTraitsWrapper<JSGradientSlider::NATIVE, JSGradientSlider> {};

jsval as_jsval(JSContext *cx, JSGradientSlider::NATIVE * theWindow);
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<acgtk::GradientSlider> & theGradientSlider);

} // namespace

#endif // TNT_JS_GRADIENT_SLIDER_INCLUDED


