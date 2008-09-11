//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef TNT_JS_HISTOGRAM
#define TNT_JS_HISTOGRAM

#include "JSDrawingArea.h"

#include <acgtk/Histogram.h>

#include <y60/jsbase/JSWrapper.h>
#include <gtkmm/window.h>
#include <gtkmm/widget.h>
#include <gtkmm/container.h>

#include <asl/base/string_functions.h>

namespace jslib {

class JSHistogram : public JSWrapper<acgtk::Histogram, asl::Ptr<acgtk::Histogram>,
                                      StaticAccessProtocol>
{
        JSHistogram();  // hide default constructor
        typedef JSDrawingArea JSBASE;
    public:
        virtual ~JSHistogram() { 
        }
        typedef acgtk::Histogram NATIVE;
        typedef asl::Ptr<acgtk::Histogram> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Histogram";
        }

        static JSFunctionSpec * Functions();

        
        enum PropertyNumbers {
            PROP_show_window = JSBASE::PROP_END,
            PROP_show_window_center,
            PROP_value_range,
            PROP_logarithmic_scale,
            PROP_mode,
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

        JSHistogram(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSHistogram & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSHistogram &>(JSHistogram::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSHistogram::NATIVE> 
    : public JSClassTraitsWrapper<JSHistogram::NATIVE, JSHistogram> {};

jsval as_jsval(JSContext *cx, JSHistogram::NATIVE * theWindow);
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<acgtk::Histogram> & theHistogram);

} // namespace

#endif  


