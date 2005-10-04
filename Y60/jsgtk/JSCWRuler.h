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
//   $RCSfile: JSCWRuler.h,v $
//   $Author: martin $
//   $Revision: 1.6 $
//   $Date: 2005/03/30 15:54:32 $
//
//
//=============================================================================

#ifndef TNT_JS_CW_RULER
#define TNT_JS_CW_RULER

#include "JSDrawingArea.h"

#include <acgtk/CWRuler.h>

#include <y60/JSWrapper.h>
#include <gtkmm/window.h>
#include <gtkmm/widget.h>
#include <gtkmm/container.h>

#include <asl/string_functions.h>

namespace jslib {

class JSCWRuler : public JSWrapper<acgtk::CWRuler, asl::Ptr<acgtk::CWRuler>,
                                      StaticAccessProtocol>
{
        JSCWRuler();  // hide default constructor
        typedef JSDrawingArea JSBASE;
    public:
        virtual ~JSCWRuler() { 
        }
        typedef acgtk::CWRuler NATIVE;
        typedef asl::Ptr<acgtk::CWRuler> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "CWRuler";
        }

        static JSFunctionSpec * Functions();

        
        enum PropertyNumbers {
            PROP_value_range = JSBASE::PROP_END,
            PROP_window_center,
            PROP_window_width,
            PROP_lower,
            PROP_upper,
            PROP_mode,
            PROP_signal_center_changed,
            PROP_signal_width_changed,
            PROP_signal_lower_changed,
            PROP_signal_upper_changed,
            PROP_MODE_THRESHOLD,
            PROP_MODE_CENTER_WIDTH,
            PROP_MODE_LOWER_UPPER,
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

        JSCWRuler(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSCWRuler & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSCWRuler &>(JSCWRuler::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSCWRuler::NATIVE> 
    : public JSClassTraitsWrapper<JSCWRuler::NATIVE, JSCWRuler> {};

jsval as_jsval(JSContext *cx, JSCWRuler::NATIVE * theWindow);
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<acgtk::CWRuler> & theCWRuler);

} // namespace

#endif  


