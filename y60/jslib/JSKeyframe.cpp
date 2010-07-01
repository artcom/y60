/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "JSKeyframe.h"

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JScppUtils.impl>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSQuaternion.h>
#include <y60/jsbase/JSWrapper.impl>

#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<QuaternionKeyframe, asl::Ptr<QuaternionKeyframe>,
         StaticAccessProtocol>;

typedef asl::QuaternionKeyframe NATIVE;

template bool convertFrom(JSContext *cx, jsval theValue, std::vector<QuaternionKeyframe> & theVector);

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
    DOC_PARAM("theTimeDelta", "", DOC_TYPE_FLOAT);
    DOC_PARAM("theSpeed", "", DOC_TYPE_FLOAT);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSKeyframe * myNewObject = 0;

    if (argc == 0) {
        OWNERPTR myNewKeyframe = OWNERPTR(new QuaternionKeyframe());
        myNewObject = new JSKeyframe(myNewKeyframe, myNewKeyframe.get());
    } else if (argc == 4) {
        asl::Vector3f myPosition;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myPosition)) {
            JS_ReportError(cx, "JSKeyFrame::Constructor: argument #1 must be a vector");
            return JS_FALSE;
        }

        asl::Quaternionf myOrienation;
        if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myOrienation)) {
            JS_ReportError(cx, "JSKeyFrame::Constructor: argument #2 must be a quaternion");
            return JS_FALSE;
        }

        float deltaTime;
        if (JSVAL_IS_VOID(argv[2]) || !convertFrom(cx, argv[2], deltaTime)) {
            JS_ReportError(cx, "JSKeyFrame::Constructor: argument #3 must be a float");
            return JS_FALSE;
        }

        float mySpeed;
        if (JSVAL_IS_VOID(argv[3]) || !convertFrom(cx, argv[3], mySpeed)) {
            JS_ReportError(cx, "JSKeyFrame::Constructor: argument #4 must be a float");
            return JS_FALSE;
        }


        OWNERPTR myNewKeyframe = OWNERPTR(new QuaternionKeyframe(myPosition, myOrienation, deltaTime, mySpeed));
        myNewObject = new JSKeyframe(myNewKeyframe, myNewKeyframe.get());
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

