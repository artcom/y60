//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef TNT_JS_GRAY_SCALE
#define TNT_JS_GRAY_SCALE

#include "JSDrawingArea.h"

#include <acgtk/GrayScale.h>

#include <y60/JSWrapper.h>
#include <gtkmm/window.h>
#include <gtkmm/widget.h>
#include <gtkmm/container.h>

#include <asl/string_functions.h>

namespace jslib {

class JSGrayScale : public JSWrapper<acgtk::GrayScale, asl::Ptr<acgtk::GrayScale>,
                                      StaticAccessProtocol>
{
        JSGrayScale();  // hide default constructor
        typedef JSDrawingArea JSBASE;
    public:
        virtual ~JSGrayScale() { 
        }
        typedef acgtk::GrayScale NATIVE;
        typedef asl::Ptr<acgtk::GrayScale> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "GrayScale";
        }

        static JSFunctionSpec * Functions();

        
        enum PropertyNumbers {
            PROP_END = JSBASE::PROP_END,
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

        JSGrayScale(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSGrayScale & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSGrayScale &>(JSGrayScale::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSGrayScale::NATIVE> 
    : public JSClassTraitsWrapper<JSGrayScale::NATIVE, JSGrayScale> {};

jsval as_jsval(JSContext *cx, JSGrayScale::NATIVE * theWindow);
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<acgtk::GrayScale> & theGrayScale);

} // namespace

#endif  


