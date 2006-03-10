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
//   $RCSfile: JSPerlinNoise.h,v $
//   $Author: ulrich $
//   $Revision: 1.5 $
//   $Date: 2005/04/22 14:58:47 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSPERLINNOISE_INCLUDED_
#define _Y60_ACXPSHELL_JSPERLINNOISE_INCLUDED_

#include "JSWrapper.h"
#include <asl/PerlinNoise.h>

namespace jslib {

class JSPerlinNoise : public JSWrapper<asl::PerlinNoise3D, asl::Ptr<asl::PerlinNoise3D>, StaticAccessProtocol>
{
        JSPerlinNoise() {}
    public:
        typedef asl::PerlinNoise3D NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "PerlinNoise";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_octavecount = -100,
            PROP_amplitudefalloff
        };

        static JSPropertySpec * Properties();
        static JSFunctionSpec * StaticFunctions();
        static JSPropertySpec * StaticProperties();

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

        JSPerlinNoise(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSPerlinNoise & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSPerlinNoise &>(JSPerlinNoise::getJSWrapper(cx,obj));
        }
    private:

};

template <>
struct JSClassTraits<asl::PerlinNoise3D> : public JSClassTraitsWrapper<asl::PerlinNoise3D, JSPerlinNoise> {};

bool convertFrom(JSContext *cx, jsval theValue, JSPerlinNoise::NATIVE & thePerlinNoise);

jsval as_jsval(JSContext *cx, JSPerlinNoise::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSPerlinNoise::OWNERPTR theOwner, JSPerlinNoise::NATIVE * thePerlinNoise);

}

#endif
