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
//   $RCSfile: JSCoordSpline.h,v $
//   $Author: christian $
//   $Revision: 1.2 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSCOORDSPLINE_INCLUDED_
#define _Y60_ACXPSHELL_JSCOORDSPLINE_INCLUDED_

#include "JSWrapper.h"
#include <asl/CoordSpline.h>


namespace jslib {

class JSCoordSpline : public JSWrapper<asl::CoordSpline, asl::Ptr<asl::CoordSpline>, StaticAccessProtocol>
{
        JSCoordSpline() {}
    public:
        typedef asl::CoordSpline NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "CoordSpline";
        }
        enum PropertyNumbers {};
        
        static JSFunctionSpec * Functions();
        static JSPropertySpec * Properties();
        static JSConstIntPropertySpec * ConstIntProperties();
        static JSPropertySpec * StaticProperties();
        static JSFunctionSpec * StaticFunctions();

        virtual unsigned long length() const {
            return 1;
        }

        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSCoordSpline(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSCoordSpline & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSCoordSpline &>(JSCoordSpline::getJSWrapper(cx,obj));
        }

    private:
};

template <>
struct JSClassTraits<asl::CoordSpline> : public JSClassTraitsWrapper<asl::CoordSpline, JSCoordSpline> {};

}

#endif

