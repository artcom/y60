//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSKEYFRAME_INCLUDED_
#define _Y60_ACXPSHELL_JSKEYFRAME_INCLUDED_

#include <Y60/JSWrapper.h>
#include <asl/CoordSpline.h>

namespace jslib {

class JSKeyframe : public JSWrapper<asl::QuaternionKeyframe, asl::Ptr<asl::QuaternionKeyframe>, StaticAccessProtocol>
{
        JSKeyframe() {}
    public:
        typedef asl::QuaternionKeyframe NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Keyframe";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_position = -100,
            PROP_orientation,
            PROP_time,
            PROP_speed
        };
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

        JSKeyframe(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}
        
        static
        JSObject * Construct(JSContext *cx, const asl::QuaternionKeyframe & theKeyframe) {
            JSKeyframe::NativeValuePtr myValue(new asl::QuaternionKeyframe(theKeyframe));
            return Base::Construct(cx, myValue, 0);
        }

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSKeyframe & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSKeyframe &>(JSKeyframe::getJSWrapper(cx,obj));
        }

    private:
};

bool convertFrom(JSContext *cx, jsval theValue, asl::QuaternionKeyframe & theKeyframe);
jsval as_jsval(JSContext *cx, const asl::QuaternionKeyframe  & theValue);

template <>
struct JSClassTraits<asl::QuaternionKeyframe> : public JSClassTraitsWrapper<asl::QuaternionKeyframe, JSKeyframe> {};

}

#endif

