//=============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSVector.h"
#include "JSBSpline.h"

namespace jslib {

    static JSBool
    setControlPoints(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        DOC_BEGIN("Set control points.");
        DOC_PARAM("theStart", "Start point", DOC_TYPE_VECTOR3F);
        DOC_PARAM("theStartAnchor", "Start point anchor", DOC_TYPE_VECTOR3F);
        DOC_PARAM("theEnd", "End point", DOC_TYPE_VECTOR3F);
        DOC_PARAM("theEndAnchor", "End point anchor", DOC_TYPE_VECTOR3F);
        DOC_RESET;
        DOC_PARAM("thePoints", "List of four points", DOC_TYPE_VECTOROFVECTOR3F);
        DOC_END;

        if (argc == 1) {
            typedef void (JSBSpline::NATIVE::*MyMethod)(const std::vector<asl::Vector3f>&);
            return Method<JSBSpline::NATIVE>::call((MyMethod)&JSBSpline::NATIVE::setControlPoints, cx, obj, argc, argv, rval);
        } else if (argc == 4) {
            typedef void (JSBSpline::NATIVE::*MyMethod)(const asl::Vector3f&, const asl::Vector3f&,
                    const asl::Vector3f&, const asl::Vector3f&);
            return Method<JSBSpline::NATIVE>::call((MyMethod)&JSBSpline::NATIVE::setControlPoints, cx, obj, argc, argv, rval);
        }

        JS_ReportError(cx, "JSBSpline::setControlPoints: bad number of arguments: expected 1 or 4");
        return JS_FALSE;
    }

    static JSBool
    calculate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        DOC_BEGIN("Sample the spline a number of times.");
        DOC_PARAM("theResolution", "Number of points to sample.", DOC_TYPE_INTEGER);
        DOC_PARAM("theEaseIn", "Ease-in value.", DOC_TYPE_FLOAT);
        DOC_PARAM("theEaseOut", "Ease-out value.", DOC_TYPE_FLOAT);
        DOC_RVAL("List of points", DOC_TYPE_VECTOROFVECTOR3F);
        DOC_END;

        if (argc < 1) {
            JS_ReportError(cx, "JSBSpline::calculate: bad number of arguments: expected at least 1");
            return JS_FALSE;
        }

        JSBSpline::NATIVE * myNative = 0;
        if (!convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative)) {
            JS_ReportError(cx, "JSBSpline: self is not a BSpline");
            return JS_FALSE;
        }

        try {
            unsigned myResolution;
            float myEaseIn = 0.0f, myEaseOut = 0.0f;

            if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myResolution)) {
                JS_ReportError(cx, "JSBSpline: argument #1 must be an integer");
                return JS_FALSE;
            }
            if (argc >= 2) {
                if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myEaseIn)) {
                    JS_ReportError(cx, "JSBSpline: argument #2 must be a float");
                    return JS_FALSE;
                }
            }
            if (argc >= 3) {
                if (JSVAL_IS_VOID(argv[2]) || !convertFrom(cx, argv[2], myEaseOut)) {
                    JS_ReportError(cx, "JSBSpline: argument #3 must be a float");
                    return JS_FALSE;
                }
            }

            *rval = as_jsval(cx, myNative->calculate(myResolution, myEaseIn, myEaseOut));
            return JS_TRUE;
        } HANDLE_CPP_EXCEPTION;
    }

    static JSBool
    evaluate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        DOC_BEGIN("Calculate a point on the spline.");
        DOC_PARAM("theCurveParameter", "Position along the curve.", DOC_TYPE_FLOAT);
        DOC_PARAM("theEaseIn", "Ease-in value.", DOC_TYPE_FLOAT);
        DOC_PARAM("theEaseOut", "Ease-out value.", DOC_TYPE_FLOAT);
        DOC_RVAL("Point on spline.", DOC_TYPE_VECTOR3F);
        DOC_END;

        if (argc < 1) {
            JS_ReportError(cx, "JSBSpline::calculate: bad number of arguments: expected at least 1");
            return JS_FALSE;
        }

        JSBSpline::NATIVE * myNative = 0;
        if (!convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative)) {
            JS_ReportError(cx, "JSBSpline: self is not a BSpline");
            return JS_FALSE;
        }

        try {
            float myCurveParameter;
            float myEaseIn = 0.0f, myEaseOut = 0.0f;

            if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myCurveParameter)) {
                JS_ReportError(cx, "JSBSpline: argument #1 must be a float");
                return JS_FALSE;
            }
            if (argc >= 2) {
                if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myEaseIn)) {
                    JS_ReportError(cx, "JSBSpline: argument #2 must be a float");
                    return JS_FALSE;
                }
            }
            if (argc >= 3) {
                if (JSVAL_IS_VOID(argv[2]) || !convertFrom(cx, argv[2], myEaseOut)) {
                    JS_ReportError(cx, "JSBSpline: argument #3 must be a float");
                    return JS_FALSE;
                }
            }

            *rval = as_jsval(cx, myNative->evaluate(myCurveParameter, myEaseIn, myEaseOut));
            return JS_TRUE;
        } HANDLE_CPP_EXCEPTION;
    }

    static JSBool
    getArcLength(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        DOC_BEGIN("Returns the arc length of the spline.");
        DOC_RVAL("Arc length.", DOC_TYPE_FLOAT);
        DOC_END;
        return Method<JSBSpline::NATIVE>::call(&JSBSpline::NATIVE::getArcLength, cx, obj, argc, argv, rval);
    }

    static JSBool
    getResult(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        DOC_BEGIN("Returns sampled points.");
        DOC_RVAL("Sampled points.", DOC_TYPE_VECTOROFVECTOR3F);
        DOC_END;
        return Method<JSBSpline::NATIVE>::call(&JSBSpline::NATIVE::getResult, cx, obj, argc, argv, rval);
    }

    JSPropertySpec * JSBSpline::Properties()
    {
        static JSPropertySpec myProperties[] = {
            {"length", PROP_length, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
            {0}
        };
        return myProperties;
    }

    JSFunctionSpec * JSBSpline::Functions() {
        static JSFunctionSpec myFunctions[] = {
            //{"name", native, nargs},
            {"setControlPoints", setControlPoints, 4},
            {"calculate", calculate, 3},
            {"evaluate", evaluate, 3},
            {"getArcLength", getArcLength, 0},
            {"getResult", getResult, 0},
            {0}
        };
        return myFunctions;
    }

    JSConstIntPropertySpec * JSBSpline::ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSPropertySpec * JSBSpline::StaticProperties() {
        static JSPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSFunctionSpec * JSBSpline::StaticFunctions() {
        static JSFunctionSpec myProperties[] = {{0}};
        return myProperties;
    }
 
    // getproperty handling
    JSBool
    JSBSpline::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        switch (theID) {
        case PROP_length:
            //*vp = as_jsval(cx, getNative().getArcLength());
            return JS_TRUE;
        }
        JS_ReportError(cx,"JSBSpline::getProperty: index %d out of range", theID);
        return JS_FALSE;
    };

    // set property
    JSBool
    JSBSpline::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        JS_ReportError(cx,"JSBSpline::setProperty: index %d out of range", theID);
        return JS_FALSE;
    }
    
    JSBool JSBSpline::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Constructor a new BSpline object");
        DOC_PARAM("theStart", "Start point", DOC_TYPE_VECTOR3F);
        DOC_PARAM("theStartAnchor", "Start point anchor", DOC_TYPE_VECTOR3F);
        DOC_PARAM("theEnd", "End point", DOC_TYPE_VECTOR3F);
        DOC_PARAM("theEndAnchor", "End point anchor", DOC_TYPE_VECTOR3F);
        DOC_RESET;
        DOC_PARAM("thePoints", "List of four points", DOC_TYPE_VECTOROFVECTOR3F);
        DOC_END;

        if (JSA_GetClass(cx, obj) != Class()) {
            JS_ReportError(cx, "Constructor for %s: bad object; did you forget a 'new'?", ClassName());
            return JS_FALSE;
        }

        JSBSpline * myNewObject = 0;
        if (argc == 0) {
            OWNERPTR myBSpline = OWNERPTR(new asl::BSpline<BSplineNumber>());
            myNewObject = new JSBSpline(myBSpline, &*myBSpline);
        } else if (argc == 1) {
            std::vector<asl::Vector3f> myPoints;
            if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myPoints)) {
                JS_ReportError(cx, "JSBSpline: argument #1 must be a vectorofvector");
                return JS_FALSE;
            }

            OWNERPTR myBSpline = OWNERPTR(new asl::BSpline<BSplineNumber>(myPoints));
            myNewObject = new JSBSpline(myBSpline, &(*myBSpline));
        } else if (argc == 4) {
            asl::Vector3f myStart, myStartAnchor;
            asl::Vector3f myEnd, myEndAnchor;

            if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myStart)) {
                JS_ReportError(cx, "JSBSpline: argument #1 must be a vector");
                return JS_FALSE;
            }
            if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myStartAnchor)) {
                JS_ReportError(cx, "JSBSpline: argument #2 must be a vector");
                return JS_FALSE;
            }
            if (JSVAL_IS_VOID(argv[2]) || !convertFrom(cx, argv[2], myEnd)) {
                JS_ReportError(cx, "JSBSpline: argument #3 must be a vector");
                return JS_FALSE;
            }
            if (JSVAL_IS_VOID(argv[3]) || !convertFrom(cx, argv[3], myEndAnchor)) {
                JS_ReportError(cx, "JSBSpline: argument #4 must be a vector");
                return JS_FALSE;
            }

            OWNERPTR myBSpline = OWNERPTR(new asl::BSpline<BSplineNumber>(myStart, myStartAnchor,
                                                                          myEnd, myEndAnchor));
            myNewObject = new JSBSpline(myBSpline, &(*myBSpline));
        } else {
            JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected 0,1,4, got %d", ClassName(), argc);
            return JS_FALSE;
        }

        if (myNewObject) {
            JS_SetPrivate(cx, obj, myNewObject);
            return JS_TRUE;
        }

        JS_ReportError(cx, "JSBSpline::Constructor: bad parameters");
        return JS_FALSE;
    }

    JSObject * JSBSpline::initClass(JSContext * cx, JSObject * theGlobalObject)
    {
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
        DOC_CREATE(JSBSpline);

        return myClass;
    }

    bool convertFrom(JSContext *cx, jsval theValue, JSBSpline::NATIVE *& theBSpline) {
        if (JSVAL_IS_OBJECT(theValue)) {
            JSObject * myArgument;
            if (JS_ValueToObject(cx, theValue, &myArgument)) {
                if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::BSpline<BSplineNumber> >::Class()) {
                    typedef JSClassTraits<asl::BSpline<BSplineNumber> >::ScopedNativeRef NativeRef;
                    NativeRef myObj(cx, myArgument);
                    theBSpline = &myObj.getNative();
                    return true;
                }
            }
        }
        return false;
    }

    jsval as_jsval(JSContext *cx, JSBSpline::OWNERPTR theOwner) {
        JSObject * myReturnObject = JSBSpline::Construct(cx, theOwner, &(*theOwner));
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSBSpline::OWNERPTR theOwner, JSBSpline::NATIVE * theBSpline) {
        JSObject * myObject = JSBSpline::Construct(cx, theOwner, theBSpline);
        return OBJECT_TO_JSVAL(myObject);
    }

}
