//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSRenderArea.h,v $
//   $Author: martin $
//   $Revision: 1.6 $
//   $Date: 2005/03/30 15:54:32 $
//
//
//=============================================================================

#ifndef _Y60_ACGTKSHELL_JSRENDERAREA_INCLUDED_
#define _Y60_ACGTKSHELL_JSRENDERAREA_INCLUDED_

#include <acgtk/RenderArea.h>

#include <y60/JSWrapper.h>
#include <gtkmm/window.h>
#include <gtkmm/widget.h>
#include <gtkmm/container.h>

#include <asl/string_functions.h>

namespace jslib {

class JSRenderArea : public JSWrapper<acgtk::RenderArea, asl::Ptr<acgtk::RenderArea>, StaticAccessProtocol> {
        JSRenderArea();  // hide default constructor
    public:
        virtual ~JSRenderArea() {
        }
        typedef acgtk::RenderArea NATIVE;
        typedef asl::Ptr<acgtk::RenderArea> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "RenderArea";
        }
        static JSFunctionSpec * Functions();

        /*
        enum PropertyNumbers {
        };
        */
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

        JSRenderArea(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);
        static void addClassProperties(JSContext * cx, JSObject * theClassProto);

        static JSRenderArea & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSRenderArea &>(JSRenderArea::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<JSRenderArea::NATIVE>
    : public JSClassTraitsWrapper<JSRenderArea::NATIVE, JSRenderArea> {};

jsval as_jsval(JSContext *cx, JSRenderArea::OWNERPTR theOwner, JSRenderArea::NATIVE * theWindow);
bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<acgtk::RenderArea> & theRenderArea);

} // namespace

#endif


