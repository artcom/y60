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

// own header
#include "JSBSpline.h"

#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSWrapper.impl>

namespace jslib {

    template class JSWrapper<asl::BSpline<BSplineNumber>, asl::Ptr<asl::BSpline<BSplineNumber> >,
             StaticAccessProtocol>;

    template <>
    struct JSClassTraits<asl::BSpline<BSplineNumber> >
        : public JSClassTraitsWrapper<asl::BSpline<BSplineNumber>, JSBSpline>
    {
    };

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

        JSBSpline::NATIVE * myNative = 0;
        if (!convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative)) {
            JS_ReportError(cx, "JSBSpline: self is not a BSpline");
            return JS_FALSE;
        }

        if (argc == 1) {
            std::vector<asl::Vector3f> myPoints;
            if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myPoints)) {
                JS_ReportError(cx, "JSBSpline: argument #1 must be a vectorofvector");
                return JS_FALSE;
            }

            myNative->setControlPoints(myPoints);
            return JS_TRUE;
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
            myNative->setControlPoints(myStart, myStartAnchor, myEnd, myEndAnchor);
            return JS_TRUE;
        }

        JS_ReportError(cx, "JSBSpline::setControlPoints: bad number of arguments: expected 1 or 4");
        return JS_FALSE;
    }

    static JSBool
    setupFromPoints(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        DOC_BEGIN("Setup BSpline from points.");
        DOC_PARAM("thePoints", "'Points'", DOC_TYPE_VECTOROFVECTOR3F);
        DOC_PARAM("theSize", "Size.", DOC_TYPE_FLOAT);
        DOC_END;

        JSBSpline::NATIVE * myNative = 0;
        if (!convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative)) {
            JS_ReportError(cx, "JSBSpline: self is not a BSpline");
            return JS_FALSE;
        }

        if (argc != 2) {
            JS_ReportError(cx, "JSBSpline::setupFromPoints: bad number of arguments: expected 2");
            return JS_FALSE;
        }

        std::vector<asl::Vector3f> myPoints;
        if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myPoints)) {
            JS_ReportError(cx, "JSBSpline: argument #1 must be a vectorofvector");
            return JS_FALSE;
        }

        float mySize;
        if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], mySize)) {
            JS_ReportError(cx, "JSBSpline: argument #2 must be a float");
            return JS_FALSE;
        }

        myNative->setupFromPoints(myPoints, mySize);
        return JS_TRUE;
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
        DOC_PARAM_OPT("theEaseIn", "Ease-in value.", DOC_TYPE_FLOAT, 0);
        DOC_PARAM_OPT("theEaseOut", "Ease-out value.", DOC_TYPE_FLOAT, 0);
        DOC_RVAL("Point on spline.", DOC_TYPE_VECTOR3F);
        DOC_END;

        if (argc < 1) {
            JS_ReportError(cx, "JSBSpline::evaluate: bad number of arguments: expected at least 1");
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
    evaluateNormal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        DOC_BEGIN("Calculate normal at point on the spline.");
        DOC_PARAM("theCurveParameter", "Position along the curve.", DOC_TYPE_FLOAT);
        DOC_PARAM_OPT("theEaseIn", "Ease-in value.", DOC_TYPE_FLOAT, 0);
        DOC_PARAM_OPT("theEaseOut", "Ease-out value.", DOC_TYPE_FLOAT, 0);
        DOC_PARAM_OPT("theUpVector", "Up-vector.", DOC_TYPE_VECTOR3F, "[0,0,1]");
        DOC_RVAL("Point on spline.", DOC_TYPE_VECTOR3F);
        DOC_END;

        if (argc < 1) {
            JS_ReportError(cx, "JSBSpline::evaluateNormal: bad number of arguments: expected at least 1");
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
            asl::Vector3f myUpVector(0.0f, 0.0f, 1.0f);

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
            if (argc >= 4) {
                if (JSVAL_IS_VOID(argv[3]) || !convertFrom(cx, argv[3], myUpVector)) {
                    JS_ReportError(cx, "JSBSpline: argument #3 must be a float");
                    return JS_FALSE;
                }
            }

            *rval = as_jsval(cx, myNative->evaluateNormal(myCurveParameter, myEaseIn, myEaseOut, myUpVector));
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
    getPosition(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        DOC_BEGIN("Returns the position along the spline for a given distance. It returns the last point on the spline, if the distance is greater than the spline length");
        DOC_PARAM("theDistance", "The distance along the spline (in meter), where to get the point from.", DOC_TYPE_FLOAT);
        DOC_RVAL("Position on the Spline.", DOC_TYPE_VECTOR3F);
        DOC_END;
        return Method<JSBSpline::NATIVE>::call(&JSBSpline::NATIVE::getPosition, cx, obj, argc, argv, rval);
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
            {"start", PROP_start, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
            {"end", PROP_end, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
            {"starthandle", PROP_starthandle, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
            {"endhandle", PROP_endhandle, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
            {"isLineSegment", PROP_isLineSegment, JSPROP_ENUMERATE|JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
            {0}
        };
        return myProperties;
    }

    JSFunctionSpec * JSBSpline::Functions() {
        static JSFunctionSpec myFunctions[] = {
            //{"name", native, nargs},
            {"setControlPoints", setControlPoints, 4},
            {"setupFromPoints", setupFromPoints, 2},
            {"calculate", calculate, 3},
            {"evaluate", evaluate, 3},
            {"evaluateNormal", evaluateNormal, 4},
            {"getArcLength", getArcLength, 0},
            {"getPosition", getPosition, 1},
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
            case PROP_start:
                *vp = as_jsval(cx, getNative().getStart());
                return JS_TRUE;
            case PROP_end:
                *vp = as_jsval(cx, getNative().getEnd());
                return JS_TRUE;
            case PROP_starthandle:
                *vp = as_jsval(cx, getNative().getStartHandle());
                return JS_TRUE;
            case PROP_endhandle:
                *vp = as_jsval(cx, getNative().getEndHandle());
                return JS_TRUE;
            case PROP_isLineSegment:
                *vp = as_jsval(cx, getNative().isLineSegment());
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
            myNewObject = new JSBSpline(myBSpline, myBSpline.get());
        } else if (argc == 1) {
            std::vector<asl::Vector3f> myPoints;
            if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myPoints)) {
                JS_ReportError(cx, "JSBSpline: argument #1 must be a vectorofvector");
                return JS_FALSE;
            }

            OWNERPTR myBSpline = OWNERPTR(new asl::BSpline<BSplineNumber>(myPoints));
            myNewObject = new JSBSpline(myBSpline, myBSpline.get());
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
            myNewObject = new JSBSpline(myBSpline, myBSpline.get());
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
        DOC_MODULE_CREATE("Math", JSBSpline);
        return myClass;
    }

    bool convertFrom(JSContext * cx, jsval theValue, JSBSpline::NATIVE & theBSpline) {
        if (JSVAL_IS_OBJECT(theValue)) {
            JSObject * myArgument;
            if (JS_ValueToObject(cx, theValue, &myArgument)) {
                if (JSA_GetClass(cx,myArgument) == JSClassTraits<asl::BSpline<BSplineNumber> >::Class()) {
                    typedef JSClassTraits<asl::BSpline<BSplineNumber> >::ScopedNativeRef NativeRef;
                    NativeRef myObj(cx, myArgument);
                    theBSpline = myObj.getNative();
                    return true;
                }
            }
        }
        return false;
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
        JSObject * myReturnObject = JSBSpline::Construct(cx, theOwner, theOwner.get());
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSBSpline::OWNERPTR theOwner, JSBSpline::NATIVE * theBSpline) {
        JSObject * myObject = JSBSpline::Construct(cx, theOwner, theBSpline);
        return OBJECT_TO_JSVAL(myObject);
    }

}
