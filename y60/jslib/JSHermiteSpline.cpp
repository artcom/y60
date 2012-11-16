/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2010, ART+COM AG Berlin, Germany <www.artcom.de>
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

// own header
#include "JSHermiteSpline.h"

#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSWrapper.impl>

namespace jslib {

    template class JSWrapper<asl::Hermite, asl::Ptr<asl::Hermite>,
             StaticAccessProtocol>;

    template <>
    struct JSClassTraits<asl::Hermite >
    : public JSClassTraitsWrapper<asl::Hermite, JSHermiteSpline>
    {
    };

    static JSBool
    init(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        DOC_BEGIN("init the spline");
        DOC_PARAM("theXValues", "x coordinates of controlpoints", DOC_TYPE_VECTOROFFLOAT);
        DOC_PARAM("theYValues", "y coordinates of controlpoints", DOC_TYPE_VECTOROFFLOAT);
        DOC_PARAM("theInitMode", "can be one of \"catmull_rom, weighted_ratio, minimize_fluctuations_forward, minimize_fluctuations\"", DOC_TYPE_STRING);
        DOC_PARAM("theAscendOnly", "ascend_only", DOC_TYPE_BOOLEAN);
        DOC_END;

        JSHermiteSpline::NATIVE * myNative = 0;
        if (!convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative)) {
            JS_ReportError(cx, "JSHermiteSpline: self is not a Hermite");
            return JS_FALSE;
        }

        if (argc == 4) {
            std::vector<float> x, y;
            asl::HermiteInitMode initMode;
            std::string initModeString;
            bool ascendOnly;

            if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], x)) {
                JS_ReportError(cx, "JSHermiteSpline: argument #1 must be an array of floats");
                return JS_FALSE;
            }
            if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], y)) {
                JS_ReportError(cx, "JSHermiteSpline: argument #2 must be an array of floats");
                return JS_FALSE;
            }
            if (JSVAL_IS_VOID(argv[2]) || !convertFrom(cx, argv[2], initModeString)) {
                JS_ReportError(cx, "JSHermiteSpline: argument #3 must be a string");
                return JS_FALSE;
            }
            if (JSVAL_IS_VOID(argv[3]) || !convertFrom(cx, argv[3], ascendOnly)) {
                JS_ReportError(cx, "JSHermiteSpline: argument #4 must be a bool");
                return JS_FALSE;
            }

            if(initModeString == "catmull_rom") {
                initMode = asl::catmull_rom;
            } else if(initModeString == "weighted_ratio") {
                initMode = asl::weighted_ratio;
            } else if(initModeString == "minimize_fluctuations_forward") {
                initMode = asl::minimize_fluctuations_forward;
            }  else if(initModeString == "minimize_fluctuations") {
                initMode = asl::minimize_fluctuations;
            } else {
                JS_ReportError(cx, "JSHermiteSpline: unsupported initMode");
                return JS_FALSE;
            }

            myNative->init(x, y, initMode, ascendOnly);
            return JS_TRUE;
        }

        JS_ReportError(cx, "JSHermiteSpline::init: bad number of arguments: expected 4");
        return JS_FALSE;
    }

    static JSBool
    evaluate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        DOC_BEGIN("Calculate y from x on the Spline.");
        DOC_PARAM("theCurveParameter", "Position along the curve.", DOC_TYPE_FLOAT);
        DOC_PARAM_OPT("order", "order of derivative", DOC_TYPE_INTEGER, 0);
        DOC_RVAL("Point on Spline.", DOC_TYPE_FLOAT);
        DOC_END;

        if (argc < 1) {
            JS_ReportError(cx, "JSHermiteSpline::evaluate: bad number of arguments: expected at least 1");
            return JS_FALSE;
        }

        JSHermiteSpline::NATIVE * myNative = 0;
        if (!convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative)) {
            JS_ReportError(cx, "JSHermiteSpline: self is not a Hermite");
            return JS_FALSE;
        }

        try {
            float myCurveParameter;

            if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myCurveParameter)) {
                JS_ReportError(cx, "JSHermiteSpline: argument #1 must be a float");
                return JS_FALSE;
            }

            int order = 0;
            if(argc == 2) {
                if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], order)) {
                    JS_ReportError(cx, "JSHermiteSpline: argument #2 must be an int");
                    return JS_FALSE;
                }
            }
            *rval = as_jsval(cx, myNative->evaluate(myCurveParameter, order));
            return JS_TRUE;
        } HANDLE_CPP_EXCEPTION;
    }

    JSPropertySpec * JSHermiteSpline::Properties()
    {
        static JSPropertySpec myProperties[] = {
            {"xFirst", PROP_xFirst, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
            {"xLast", PROP_xLast, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
            {"yFirst", PROP_yFirst, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
            {"yLast", PROP_yLast, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
            {"dim", PROP_dim, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
            {0}
        };
        return myProperties;
    }

    JSFunctionSpec * JSHermiteSpline::Functions() {
        static JSFunctionSpec myFunctions[] = {
            //{"name", native, nargs},
            {"init", init, 4},
            {"evaluate", evaluate, 2},
            {0}
        };
        return myFunctions;
    }

    JSConstIntPropertySpec * JSHermiteSpline::ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSPropertySpec * JSHermiteSpline::StaticProperties() {
        static JSPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSFunctionSpec * JSHermiteSpline::StaticFunctions() {
        static JSFunctionSpec myProperties[] = {{0}};
        return myProperties;
    }

    // getproperty handling
    JSBool
    JSHermiteSpline::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        switch (theID) {
            case PROP_xFirst:
                *vp = as_jsval(cx, getNative().x_first());
                return JS_TRUE;
            case PROP_xLast:
                *vp = as_jsval(cx, getNative().x_last());
                return JS_TRUE;
            case PROP_yFirst:
                *vp = as_jsval(cx, getNative().y_first());
                return JS_TRUE;
            case PROP_yLast:
                *vp = as_jsval(cx, getNative().y_last());
                return JS_TRUE;
            case PROP_dim:
                *vp = as_jsval(cx, getNative().dim());
                return JS_TRUE;
        }
        JS_ReportError(cx,"JSHermiteSpline::getProperty: index %d out of range", theID);
        return JS_FALSE;
    };

    // set property
    JSBool
    JSHermiteSpline::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        JS_ReportError(cx,"JSHermiteSpline::setProperty: index %d out of range", theID);
        return JS_FALSE;
    }

    JSBool JSHermiteSpline::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Constructor a new Hermite object");
        DOC_END;

        if (JSA_GetClass(cx, obj) != Class()) {
            JS_ReportError(cx, "Constructor for %s: bad object; did you forget a 'new'?", ClassName());
            return JS_FALSE;
        }

        JSHermiteSpline * myNewObject = 0;
        if (argc == 0) {
            OWNERPTR myBSpline = OWNERPTR(new asl::Hermite);
            myNewObject = new JSHermiteSpline(myBSpline, myBSpline.get());
        } else {
            JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected 0, got %d", ClassName(), argc);
            return JS_FALSE;
        }

        if (myNewObject) {
            JS_SetPrivate(cx, obj, myNewObject);
            return JS_TRUE;
        }

        JS_ReportError(cx, "JSHermiteSpline::Constructor: bad parameters");
        return JS_FALSE;
    }

    JSObject * JSHermiteSpline::initClass(JSContext * cx, JSObject * theGlobalObject)
    {
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
        DOC_MODULE_CREATE("Math", JSHermiteSpline);
        return myClass;
    }

    bool convertFrom(JSContext * cx, jsval theValue, JSHermiteSpline::NATIVE & theHermiteSpline) {
        if (JSVAL_IS_OBJECT(theValue)) {
            JSObject * myArgument;
            if (JS_ValueToObject(cx, theValue, &myArgument)) {
                if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Hermite >::Class()) {
                    typedef JSClassTraits<asl::Hermite >::ScopedNativeRef NativeRef;
                    NativeRef myObj(cx, myArgument);
                    theHermiteSpline = myObj.getNative();
                    return true;
                }
            }
        }
        return false;
    }

    bool convertFrom(JSContext *cx, jsval theValue, JSHermiteSpline::NATIVE *& theHermiteSpline) {
        if (JSVAL_IS_OBJECT(theValue)) {
            JSObject * myArgument;
            if (JS_ValueToObject(cx, theValue, &myArgument)) {
                if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::Hermite >::Class()) {
                    typedef JSClassTraits<asl::Hermite >::ScopedNativeRef NativeRef;
                    NativeRef myObj(cx, myArgument);
                    theHermiteSpline = &myObj.getNative();
                    return true;
                }
            }
        }
        return false;
    }

    jsval as_jsval(JSContext *cx, JSHermiteSpline::OWNERPTR theOwner) {
        JSObject * myReturnObject = JSHermiteSpline::Construct(cx, theOwner, theOwner.get());
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSHermiteSpline::OWNERPTR theOwner, JSHermiteSpline::NATIVE * theHermiteSpline) {
        JSObject * myObject = JSHermiteSpline::Construct(cx, theOwner, theHermiteSpline);
        return OBJECT_TO_JSVAL(myObject);
    }

}
