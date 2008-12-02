//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSIMAGE_INCLUDED_
#define _Y60_ACGTKSHELL_JSIMAGE_INCLUDED_

#include "JSMisc.h"

#include <asl/base/string_functions.h>
#include <y60/jsbase/JSWrapper.h>

#if defined(_MSC_VER)
#pragma warning(push,1)
#endif //defined(_MSC_VER)
#include <gtkmm/image.h>
#if defined(_MSC_VER)
#pragma warning(push,1)
#endif //defined(_MSC_VER)


namespace jslib {

class JSImage : public JSWrapper<Gtk::Image, asl::Ptr<Gtk::Image>, StaticAccessProtocol> {
    private:
        JSImage();  // hide default constructor
        typedef JSMisc JSBASE;
    public:
        virtual ~JSImage() {
        }
        typedef Gtk::Image NATIVE;
        typedef asl::Ptr<Gtk::Image> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Image";
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

        JSImage(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSImage & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSImage &>(JSImage::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSImage::NATIVE>
    : public JSClassTraitsWrapper<JSImage::NATIVE, JSImage> {};

jsval as_jsval(JSContext *cx, JSImage::OWNERPTR theOwner, JSImage::NATIVE * theImage);

} // namespace

#endif

