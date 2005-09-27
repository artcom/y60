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
//   $RCSfile: JSImage.h,v $
//   $Author: david $
//   $Revision: 1.2 $
//   $Date: 2005/02/07 18:17:21 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSIMAGE_INCLUDED_
#define _Y60_ACGTKSHELL_JSIMAGE_INCLUDED_

#include "JSMisc.h"

#include <asl/string_functions.h>
#include <y60/JSWrapper.h>

#include <gtkmm/image.h>


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

