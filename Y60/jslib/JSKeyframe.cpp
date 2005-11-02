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
//   $RCSfile: JSKeyframe.cpp,v $
//   $Author: christian $
//   $Revision: 1.4 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSKeyframe.h"
#include "JScppUtils.h"
#include "JSNode.h"
#include "JSVector.h"
#include "JSQuaternion.h"

#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

typedef asl::QuaternionKeyframe NATIVE;


JSFunctionSpec *
JSKeyframe::Functions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

JSPropertySpec *
JSKeyframe::Properties() {
    static JSPropertySpec myProperties[] = {
        {"position", PROP_position, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"orientation", PROP_orientation, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"time", PROP_time, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"speed", PROP_speed, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSKeyframe::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSKeyframe::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSKeyframe::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSKeyframe::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_position:
                *vp = as_jsval(cx, getNative().getPosition());
                return JS_TRUE;
            case PROP_orientation:
                *vp = as_jsval(cx, getNative().getOrientation());
                return JS_TRUE;
            case PROP_time:
                *vp = as_jsval(cx, getNative().getTime());
                return JS_TRUE;
            case PROP_speed:
                *vp = as_jsval(cx, getNative().getSpeed());
                return JS_TRUE;
            default:
            JS_ReportError(cx,"JSKeyframe::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }

}


// setproperty handling
JSBool
JSKeyframe::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    jsval dummy;
    switch (theID) {
        case PROP_position:
            return Method<NATIVE>::call(&NATIVE::setPosition, cx, obj, 1, vp, &dummy);
        case PROP_orientation:
            return Method<NATIVE>::call(&NATIVE::setOrientation, cx, obj, 1, vp, &dummy);
        case PROP_time:
            return Method<NATIVE>::call(&NATIVE::setTime, cx, obj, 1, vp, &dummy);
        case PROP_speed:
            return Method<NATIVE>::call(&NATIVE::setSpeed, cx, obj, 1, vp, &dummy);
        default:
            JS_ReportError(cx,"JSKeyframe::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}



JSBool
JSKeyframe::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Construct a Keyframe");
    DOC_PARAM("thePosition", "", DOC_TYPE_VECTOR3F);
    DOC_PARAM("theOrientation", "", DOC_TYPE_QUATERNIONF);
    DOC_PARAM("theTimestamp", "", DOC_TYPE_FLOAT);
    DOC_PARAM("theSpeed", "", DOC_TYPE_FLOAT);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSKeyframe * myNewObject = 0;

    if (argc == 0) {
        OWNERPTR myNewKeyframe = OWNERPTR(new QuaternionKeyframe());
        myNewObject = new JSKeyframe(myNewKeyframe, &(*myNewKeyframe));
    } else if (argc == 4) {
        asl::Vector3f myPosition;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myPosition)) {
            JS_ReportError(cx, "JSStringMover::Constructor: argument #1 must be a vector");
            return JS_FALSE;
        }

//        asl::Vector3f myOrienation;
        asl::Quaternionf myOrienation;
        if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myOrienation)) {
            JS_ReportError(cx, "JSStringMover::Constructor: argument #2 must be a vector");
            return JS_FALSE;
        }

        float myTimeStamp;
        if (JSVAL_IS_VOID(argv[2]) || !convertFrom(cx, argv[2], myTimeStamp)) {
            JS_ReportError(cx, "JSStringMover::Constructor: argument #3 must be a float");
            return JS_FALSE;
        }
        float mySpeed;
        if (JSVAL_IS_VOID(argv[3]) || !convertFrom(cx, argv[3], mySpeed)) {
            JS_ReportError(cx, "JSStringMover::Constructor: argument #4 must be a float");
            return JS_FALSE;
        }


        OWNERPTR myNewKeyframe = OWNERPTR(new QuaternionKeyframe(myPosition, myOrienation, myTimeStamp, mySpeed));
        myNewObject = new JSKeyframe(myNewKeyframe, &(*myNewKeyframe));
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 4 (position, orienation, timestamp, speed) %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSKeyframe::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSKeyframe::initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor,
                Properties(), Functions());
        DOC_CREATE(JSKeyframe);
        return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, asl::QuaternionKeyframe & theKeyframe) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::QuaternionKeyframe >::Class()) {
                theKeyframe = JSClassTraits<asl::QuaternionKeyframe >::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, const asl::QuaternionKeyframe  & theValue) {
    JSObject * myReturnObject = JSKeyframe::Construct(cx, theValue);
    return OBJECT_TO_JSVAL(myReturnObject);
}

}

