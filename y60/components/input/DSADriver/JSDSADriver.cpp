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
*/
//
// Dieters Sensor Array...
//
//=============================================================================

#include "JSDSADriver.h"

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSNode.h>
#include <y60/input/EventDispatcher.h>

namespace jslib {

    static JSBool
    calibrate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;
        try {
            unsigned int myPortId;
            std::string myFileName;
            if (!convertFrom(cx, argv[0], myPortId )) {
                JS_ReportError(cx, "JSDSADriver::calibrate(): Argument #1 must be a integer");
                return JS_FALSE;
            }
            if (!convertFrom(cx, argv[1], myFileName )) {
                JS_ReportError(cx, "JSDSADriver::calibrate(): Argument #2 must be a filename");
                return JS_FALSE;
            }
            JSDSADriver::OWNERPTR myNative;
            if (!convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative)) {
                JS_ReportError(cx, "JSDSADriver: self is not a DSADriver");
                return JS_FALSE;
            }

            myNative->calibrate(myPortId, myFileName);
            return JS_TRUE;
        } HANDLE_CPP_EXCEPTION;
    }

    static JSBool
    onUpdateSettings(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Define a callback to be called when a given settings node changes.");
        DOC_PARAM("theConfigNode", "", DOC_TYPE_NODE);
        DOC_END;
        try {
            if (argc != 1) {
                JS_ReportError(cx, "JSDSADriver::onUpdateSettings(): Wrong number of arguments. One (ConfigNode) expected");
                return JS_FALSE;
            }

            dom::NodePtr myConfigNode;
            if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myConfigNode)) {
                JS_ReportError(cx, "JSDSADriver::onUpdateSettings(): Argument #1 must be a node");
                return JS_FALSE;
            }

            JSDSADriver::OWNERPTR myNative;
            if (!convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative)) {
                JS_ReportError(cx, "JSDSADriver: self is not a DSADriver");
                return JS_FALSE;
            }
            myNative->onUpdateSettings(myConfigNode);
            return JS_TRUE;
        } HANDLE_CPP_EXCEPTION;
    }

    JSFunctionSpec *
    JSDSADriver::Functions() {
        static JSFunctionSpec myFunctions[] = {
            // name                  native            nargs
            {"calibrate",            calibrate,         2},
            {"onUpdateSettings",     onUpdateSettings,  1},
            {0}
        };
        return myFunctions;
    }

    JSConstIntPropertySpec *
    JSDSADriver::ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSPropertySpec *
    JSDSADriver::Properties() {
        static JSPropertySpec myProperties[] = {
            {"status",    PROP_status,   JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {"statusInterval",    PROP_statusInterval,   JSPROP_ENUMERATE|JSPROP_PERMANENT},
            {0}
        };
        return myProperties;
    }

    JSPropertySpec *
    JSDSADriver::StaticProperties() {
        static JSPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSFunctionSpec *
    JSDSADriver::StaticFunctions() {
        static JSFunctionSpec myFunctions[] = {{0}};
        return myFunctions;
    }

    // getproperty handling
    JSBool
    JSDSADriver::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        JSDSADriver::OWNERPTR myNative;
        if (!convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative)) {
            JS_ReportError(cx, "JSDSADriver: self is not a DSADriver");
            return JS_FALSE;
        }
        switch (theID) {
            case PROP_status:
                *vp = as_jsval(cx, myNative->getStatus());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSDSADriver::getProperty: index %d out of range", theID);
                return JS_FALSE;
        }
    }

    // setproperty handling
    JSBool
    JSDSADriver::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        jsval dummy;
        switch (theID) {
            case PROP_statusInterval:
                    return Method<JSDSADriver::NATIVE>::call(&JSDSADriver::NATIVE::setStatusInterval, cx, obj, 1, vp, &dummy);
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSDSADriver::setPropertySwitch: index %d out of range", theID);
        }
        return JS_FALSE;
    }

    JSBool
    JSDSADriver::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Creates a DSADriver");
        DOC_END;
        try {
            if (JSA_GetClass(cx,obj) != Class()) {
                JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
                return JS_FALSE;
            }

            OWNERPTR myNewNative;
            myNewNative = OWNERPTR(new y60::DSADriver());
            y60::EventDispatcher::get().addSource(myNewNative.get());
            JSDSADriver * myNewObject = new JSDSADriver(myNewNative, myNewNative.get());
            if (myNewObject) {
                JS_SetPrivate(cx, obj, myNewObject);
                return JS_TRUE;
            } else  {
                JS_ReportError(cx,"JSDSADriver::Constructor: bad parameters");
                return JS_FALSE;
            }
        } HANDLE_CPP_EXCEPTION;
    }

    JSObject *
    JSDSADriver::initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
        DOC_MODULE_CREATE("DSADriver", JSDSADriver);
        return myClass;
    }

    bool convertFrom(JSContext *cx, jsval theValue, JSDSADriver::OWNERPTR & theNativePtr) {
        if (JSVAL_IS_OBJECT(theValue)) {
            JSObject * myArgument;
            if (JS_ValueToObject(cx, theValue, &myArgument)) {
                if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSDSADriver::NATIVE>::Class()) {
                    theNativePtr = JSClassTraits<JSDSADriver::NATIVE>::getNativeOwner(cx,myArgument);
                    return true;
                }
            }
        }
        return false;
    }


    bool convertFrom(JSContext *cx, jsval theValue, JSDSADriver::NATIVE & theNative) {
        if (JSVAL_IS_OBJECT(theValue)) {
            JSObject * myArgument;
            if (JS_ValueToObject(cx, theValue, &myArgument)) {
                if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSDSADriver::NATIVE >::Class()) {
                    theNative = JSClassTraits<JSDSADriver::NATIVE>::getNativeRef(cx,myArgument);
                    return true;
                }
            }
        }
        return false;
    }

    jsval as_jsval(JSContext *cx, JSDSADriver::OWNERPTR theOwner) {
        JSObject * myReturnObject = JSDSADriver::Construct(cx, theOwner, theOwner.get());
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSDSADriver::OWNERPTR theOwner, JSDSADriver::NATIVE * theDSADriver) {
        JSObject * myObject = JSDSADriver::Construct(cx, theOwner, theDSADriver);
        return OBJECT_TO_JSVAL(myObject);
    }
}

