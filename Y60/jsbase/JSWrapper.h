//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSWRAPPER_H_INCLUDED_
#define _Y60_ACXPSHELL_JSWRAPPER_H_INCLUDED_

#include "Documentation.h"
#include "JScppUtils.h"
#include "ArgumentHolder.h"
#include <js/spidermonkey/jspubtd.h>
#include <js/spidermonkey/jsapi.h>
#include <js/spidermonkey/jscntxt.h>
#include <js/spidermonkey/jsgc.h>
#include <js/spidermonkey/jslock.h>
#include <js/spidermonkey/jsnum.h>
#include <asl/dom/Nodes.h>
#include <asl/base/Singleton.h>

#include <set>


namespace jslib {

template <class CLASS, class METHOD, class RESULT>
struct IfResult {
    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, T2 theArg2, T3 theArg3,
                T4 theArg4, T5 theArg5, T6 theArg6, T7 theArg7)
        {
            ResultConverter<RESULT>::storeResult(theJSArgs, (theObject.*theMethod)(
                        theArg0, theArg1, theArg2, theArg3,
                        theArg4, theArg5, theArg6, theArg7));
        }
    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, T2 theArg2, T3 theArg3,
                T4 theArg4, T5 theArg5, T6 theArg6, NoArgument)
        {
            ResultConverter<RESULT>::storeResult(theJSArgs, (theObject.*theMethod)(
                        theArg0, theArg1, theArg2, theArg3, theArg4, theArg5, theArg6));
        }
    template <class T0, class T1, class T2, class T3, class T4, class T5>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, T2 theArg2, T3 theArg3,
                T4 theArg4, T5 theArg5, NoArgument, NoArgument)
        {
            ResultConverter<RESULT>::storeResult(theJSArgs, (theObject.*theMethod)(
                        theArg0, theArg1, theArg2, theArg3, theArg4, theArg5));
        }
    template <class T0, class T1, class T2, class T3, class T4>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, T2 theArg2, T3 theArg3,
                T4 theArg4, NoArgument, NoArgument, NoArgument)
        {
            ResultConverter<RESULT>::storeResult(theJSArgs, (theObject.*theMethod)(
                        theArg0, theArg1, theArg2, theArg3, theArg4));
        }
    template <class T0, class T1, class T2, class T3>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, T2 theArg2, T3 theArg3,
                NoArgument, NoArgument, NoArgument, NoArgument)
        {
            ResultConverter<RESULT>::storeResult(theJSArgs, (theObject.*theMethod)(theArg0, theArg1, theArg2, theArg3));
        }
    template <class T0, class T1, class T2>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, T2 theArg2, NoArgument,
                NoArgument, NoArgument, NoArgument, NoArgument)
        {
            ResultConverter<RESULT>::storeResult(theJSArgs, (theObject.*theMethod)(theArg0, theArg1, theArg2));
        }
    template <class T0, class T1>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, NoArgument, NoArgument,
                NoArgument, NoArgument, NoArgument, NoArgument)
        {
            ResultConverter<RESULT>::storeResult(theJSArgs, (theObject.*theMethod)(theArg0, theArg1));
        }
    template <class T0>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, NoArgument, NoArgument, NoArgument,
                NoArgument, NoArgument, NoArgument, NoArgument)
        {
            ResultConverter<RESULT>::storeResult(theJSArgs, (theObject.*theMethod)(theArg0));
        }
    static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
            NoArgument, NoArgument, NoArgument, NoArgument,
            NoArgument, NoArgument, NoArgument, NoArgument)
    {
        ResultConverter<RESULT>::storeResult(theJSArgs, (theObject.*theMethod)());
    }
};

template <class CLASS, class METHOD>
struct IfResult<CLASS, METHOD, void> {
    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, T2 theArg2, T3 theArg3,
                T4 theArg4, T5 theArg5, T6 theArg6, T7 theArg7)
        {
            (theObject.*theMethod)(theArg0, theArg1, theArg2, theArg3,
                                   theArg4, theArg5, theArg6, theArg7);
        }
    template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, T2 theArg2, T3 theArg3,
                T4 theArg4, T5 theArg5, T6 theArg6, NoArgument)
        {
            (theObject.*theMethod)(theArg0, theArg1, theArg2, theArg3, theArg4, theArg5, theArg6);
        }
    template <class T0, class T1, class T2, class T3, class T4, class T5>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, T2 theArg2, T3 theArg3,
                T4 theArg4, T5 theArg5, NoArgument, NoArgument)
        {
            (theObject.*theMethod)(theArg0, theArg1, theArg2, theArg3, theArg4, theArg5);
        }
    template <class T0, class T1, class T2, class T3, class T4>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, T2 theArg2, T3 theArg3,
                T4 theArg4, NoArgument, NoArgument, NoArgument)
        {
            (theObject.*theMethod)(theArg0, theArg1, theArg2, theArg3, theArg4);
        }
    template <class T0, class T1, class T2, class T3>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, T2 theArg2, T3 theArg3,
                NoArgument, NoArgument, NoArgument, NoArgument)
        {
            (theObject.*theMethod)(theArg0, theArg1, theArg2, theArg3);
        }
    template <class T0, class T1, class T2>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, T2 theArg2, NoArgument,
                NoArgument, NoArgument, NoArgument, NoArgument)
        {
            (theObject.*theMethod)(theArg0, theArg1, theArg2);
        }
    template <class T0, class T1>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, T1 theArg1, NoArgument, NoArgument,
                NoArgument, NoArgument, NoArgument, NoArgument)
        {
            (theObject.*theMethod)(theArg0, theArg1);
        }
    template <class T0>
        static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
                T0 theArg0, NoArgument, NoArgument, NoArgument,
                NoArgument, NoArgument, NoArgument, NoArgument)
        {
            (theObject.*theMethod)(theArg0);
        }
    static void call(JSCallArgs & theJSArgs, CLASS & theObject, METHOD theMethod,
            NoArgument, NoArgument, NoArgument, NoArgument,
            NoArgument, NoArgument, NoArgument, NoArgument)
    {
        (theObject.*theMethod)();
    }
};

template <class CLASS, class METHOD, class RESULT_TYPE=NoResult,
            class ARG0=NoArgument,
            class ARG1=NoArgument,
            class ARG2=NoArgument,
            class ARG3=NoArgument,
            class ARG4=NoArgument,
            class ARG5=NoArgument,
            class ARG6=NoArgument,
            class ARG7=NoArgument
>
struct AdapterCall {

    typedef typename Argument<RESULT_TYPE>::PlainType PlainResult;

    AdapterCall(METHOD theMethod, JSCallArgs & theJSArgs)
        :
        _myClass(JSClassTraits<CLASS>::openNativeRef(theJSArgs.cx, theJSArgs.obj)),
        _myMethod(theMethod),
        _myArgHolder0(theJSArgs, 0),
        _myArgHolder1(theJSArgs, 1),
        _myArgHolder2(theJSArgs, 2),
        _myArgHolder3(theJSArgs, 3),
        _myArgHolder4(theJSArgs, 4),
        _myArgHolder5(theJSArgs, 5),
        _myArgHolder6(theJSArgs, 6),
        _myArgHolder7(theJSArgs, 7)
    {
        IfResult<CLASS, METHOD, PlainResult>::call(theJSArgs, _myClass, _myMethod,
            _myArgHolder0.getArg(),
            _myArgHolder1.getArg(),
            _myArgHolder2.getArg(),
            _myArgHolder3.getArg(),
            _myArgHolder4.getArg(),
            _myArgHolder5.getArg(),
            _myArgHolder6.getArg(),
            _myArgHolder7.getArg()
         );
        JSClassTraits<CLASS>::closeNativeRef(theJSArgs.cx, theJSArgs.obj);
    }
    ~AdapterCall() {
    }


private:
    METHOD _myMethod;
    ArgumentHolder<ARG0> _myArgHolder0;
    ArgumentHolder<ARG1> _myArgHolder1;
    ArgumentHolder<ARG2> _myArgHolder2;
    ArgumentHolder<ARG3> _myArgHolder3;
    ArgumentHolder<ARG4> _myArgHolder4;
    ArgumentHolder<ARG5> _myArgHolder5;
    ArgumentHolder<ARG6> _myArgHolder6;
    ArgumentHolder<ARG7> _myArgHolder7;
    CLASS & _myClass;
};

template <class CLASS, class METHOD, class RESULT_TYPE=NoResult,
            class ARG0=NoArgument,
            class ARG1=NoArgument,
            class ARG2=NoArgument,
            class ARG3=NoArgument,
            class ARG4=NoArgument,
            class ARG5=NoArgument,
            class ARG6=NoArgument,
            class ARG7=NoArgument
>
struct ConstAdapterCall {

    typedef typename Argument<RESULT_TYPE>::PlainType PlainResult;

    ConstAdapterCall(METHOD theMethod, JSCallArgs & theJSArgs)
        :
        _myClass(JSClassTraits<CLASS>::getNativeRef(theJSArgs.cx, theJSArgs.obj)),
        _myMethod(theMethod),
        _myArgHolder0(theJSArgs, 0),
        _myArgHolder1(theJSArgs, 1),
        _myArgHolder2(theJSArgs, 2),
        _myArgHolder3(theJSArgs, 3),
        _myArgHolder4(theJSArgs, 4),
        _myArgHolder5(theJSArgs, 5),
        _myArgHolder6(theJSArgs, 6),
        _myArgHolder7(theJSArgs, 7)
    {
        IfResult<const CLASS, METHOD, PlainResult>::call(theJSArgs, _myClass, _myMethod,
            _myArgHolder0.getArg(),
            _myArgHolder1.getArg(),
            _myArgHolder2.getArg(),
            _myArgHolder3.getArg(),
            _myArgHolder4.getArg(),
            _myArgHolder5.getArg(),
            _myArgHolder6.getArg(),
            _myArgHolder7.getArg()
         );
    }
    ~ConstAdapterCall() {
    }


private:
    METHOD _myMethod;
    ArgumentHolder<ARG0> _myArgHolder0;
    ArgumentHolder<ARG1> _myArgHolder1;
    ArgumentHolder<ARG2> _myArgHolder2;
    ArgumentHolder<ARG3> _myArgHolder3;
    ArgumentHolder<ARG4> _myArgHolder4;
    ArgumentHolder<ARG5> _myArgHolder5;
    ArgumentHolder<ARG6> _myArgHolder6;
    ArgumentHolder<ARG7> _myArgHolder7;
    const CLASS & _myClass;
};

template <class CLASS, class METHOD>
struct MethodType {
    // with return value, non const
    template <class RESULT_TYPE>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, RESULT_TYPE>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1, class ARG2>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1, ARG2), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1, ARG2>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1, class ARG2, class ARG3>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1, ARG2, ARG3>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1, class ARG2, class ARG3, class ARG4>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3, ARG4), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1, ARG2, ARG3, ARG4>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3, ARG4, ARG5), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1, class ARG2, class ARG3, class ARG4, class ARG5, class ARG6>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1, class ARG2, class ARG3, class ARG4, class ARG5, class ARG6, class ARG7>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7>(theMethod, myJSArgs);
        }

    // with return value, const
    template <class RESULT_TYPE>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)() const, JSCallArgs & myJSArgs) {
            ConstAdapterCall<CLASS, METHOD, RESULT_TYPE>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0) const, JSCallArgs & myJSArgs) {
            ConstAdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1) const, JSCallArgs & myJSArgs) {
            ConstAdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1, class ARG2>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1, ARG2) const, JSCallArgs & myJSArgs) {
            ConstAdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1, ARG2>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1, class ARG2, class ARG3>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3) const, JSCallArgs & myJSArgs) {
            ConstAdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1, ARG2, ARG3>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1, class ARG2, class ARG3, class ARG4>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3, ARG4) const, JSCallArgs & myJSArgs) {
            ConstAdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1, ARG2, ARG3, ARG4>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3, ARG4, ARG5) const, JSCallArgs & myJSArgs) {
            ConstAdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1, class ARG2, class ARG3,
    class ARG4, class ARG5, class ARG6>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) const, JSCallArgs & myJSArgs) {
            ConstAdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6>(theMethod, myJSArgs);
        }
    template <class RESULT_TYPE, class ARG0, class ARG1, class ARG2, class ARG3,
    class ARG4, class ARG5, class ARG6, class ARG7>
        static void callMethodType(RESULT_TYPE (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7) const, JSCallArgs & myJSArgs) {
            ConstAdapterCall<CLASS, METHOD, RESULT_TYPE, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7>(theMethod, myJSArgs);
        }

    // here the void result types
        static void
        callMethodType(void (CLASS::*theMethod)(void), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, NoResult>(theMethod, myJSArgs);
        }
    template <class ARG0>
        static void
        callMethodType(void (CLASS::*theMethod)(ARG0), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, NoResult, ARG0>(theMethod, myJSArgs);
        }
    template <class ARG0, class ARG1>
        static void
        callMethodType(void (CLASS::*theMethod)(ARG0, ARG1), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, NoResult, ARG0, ARG1>(theMethod, myJSArgs);
        }
    template <class ARG0, class ARG1, class ARG2>
        static void
        callMethodType(void (CLASS::*theMethod)(ARG0, ARG1, ARG2), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, NoResult, ARG0, ARG1, ARG2>(theMethod, myJSArgs);
        }
    template <class ARG0, class ARG1, class ARG2, class ARG3>
        static void
        callMethodType(void (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, NoResult, ARG0, ARG1, ARG2, ARG3>(theMethod, myJSArgs);
        }
    template <class ARG0, class ARG1, class ARG2, class ARG3, class ARG4>
        static void
        callMethodType(void (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3, ARG4), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, NoResult, ARG0, ARG1, ARG2, ARG3, ARG4>(theMethod, myJSArgs);
        }
    template <class ARG0, class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
        static void
        callMethodType(void (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3, ARG4, ARG5), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, NoResult, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5>(theMethod, myJSArgs);
        }
    template <class ARG0, class ARG1, class ARG2, class ARG3, class ARG4, class ARG5, class ARG6>
        static void
        callMethodType(void (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, NoResult, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6>(theMethod, myJSArgs);
        }
    template <class ARG0, class ARG1, class ARG2, class ARG3, class ARG4, class ARG5, class ARG6, class ARG7>
        static void
        callMethodType(void (CLASS::*theMethod)(ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7), JSCallArgs & myJSArgs) {
            AdapterCall<CLASS, METHOD, NoResult, ARG0, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7>(theMethod, myJSArgs);
        }
};

template <class CLASS>
struct Method {
    template <class METHOD>
    static JSBool
    call(METHOD theMethod, JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        try {
            JSRuntime * myRuntime=JS_GetRuntime(cx);(void)myRuntime;
            JSCallArgs myJSArgs( cx, obj, argc, argv, rval);
            MethodType<CLASS, METHOD>::callMethodType(theMethod, myJSArgs);
            return JS_TRUE;
        } HANDLE_CPP_EXCEPTION;
    }

    // returns undefined if native function returns false, otherwise returns
    // what the native function returned in last reference argument
    // bool = Method(ARG0, RESULT_TYPE)
    template <class RESULT_TYPE, class ARG0>
        static JSBool
        callRefResult(bool (CLASS::*theMethod)(const ARG0 &, RESULT_TYPE &) ,
        JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        try {
            if (argc != 1) {
                JS_ReportError(cx,"JSNode::callRefResult: wrong number of parameters: %d, 1 expected", argc);
                return JS_FALSE;
            }
            ARG0 myArg0;
            RESULT_TYPE myResult;

            if (convertFrom(cx, argv[0], myArg0))
            {
                typename JSClassTraits<CLASS>::ScopedNativeRef myObj(cx, obj);
                if (((myObj.getNative()).*theMethod)(myArg0, myResult)) {
                    *rval = as_jsval(cx, myResult);
                } else {
                    *rval = JSVAL_VOID;
                }
                return JS_TRUE;
            }
            return JS_FALSE;
        } HANDLE_CPP_EXCEPTION;
    }

    // returns undefined if native function returns false, otherwise returns
    // what the native function returned in last reference argument
    //  bool = Method(const ARG0 &, const ARG1 &, RESULT_TYPE)
    template <class RESULT_TYPE, class ARG0, class ARG1>
        static JSBool
        callRefResult(bool (CLASS::*theMethod)(const ARG0 &, const ARG1 &, RESULT_TYPE &) ,
        JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        try {
            if (argc != 2) {
                JS_ReportError(cx,"JSNode::callMethod: wrong number of parameters: %d, 2 expected", argc);
                return JS_FALSE;
            }
            ARG0 myArg0;
            ARG1 myArg1;
            RESULT_TYPE myResult;

            if (convertFrom(cx, argv[0], myArg0) &&
                convertFrom(cx, argv[1], myArg1) )
            {
                typename JSClassTraits<CLASS>::ScopedNativeRef myObj(cx, obj);
                if (((myObj.getNative()).*theMethod)(myArg0, myArg1, myResult)) {
                    *rval = as_jsval(cx, myResult);
                } else {
                    *rval = JSVAL_VOID;
                }
                return JS_TRUE;
            }
            return JS_FALSE;
        } HANDLE_CPP_EXCEPTION;
    }
    // returns undefined if native function returns false, otherwise returns
    // what the native function returned in last reference argument
    //  bool = Method(const ARG0 &, const ARG1 &, ARG2 &, RESULT_TYPE)
    template <class RESULT_TYPE, class ARG0, class ARG1, class ARG2>
        static JSBool
        callRefResult(bool (CLASS::*theMethod)(const ARG0 &, const ARG1 &, ARG2 &, RESULT_TYPE &) ,
        JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
    {
        try {
            if (argc != 3) {
                JS_ReportError(cx,"JSNode::callMethod: wrong number of parameters: %d, 2 expected", argc);
                return JS_FALSE;
            }
            ARG0 myArg0;
            ARG1 myArg1;
            ARG2 myArg2;
            RESULT_TYPE myResult;

            if (convertFrom(cx, argv[0], myArg0) &&
                convertFrom(cx, argv[1], myArg1) &&
                convertFrom(cx, argv[2], myArg2) )
            {
                typename JSClassTraits<CLASS>::ScopedNativeRef myObj(cx, obj);
                if (((myObj.getNative()).*theMethod)(myArg0, myArg1, myArg2, myResult)) {
                    *rval = as_jsval(cx, myResult);
                } else {
                    *rval = JSVAL_VOID;
                }
                return JS_TRUE;
            }
            return JS_FALSE;
        } HANDLE_CPP_EXCEPTION;
    }
};

template <class NATIVE, class VALUEPTR>
struct ValueAccessProtocol {
    static NATIVE & openWriteableValue(NATIVE &, VALUEPTR v) {
        return v->openWriteableValue();
    }
    static void closeWriteableValue(NATIVE &, VALUEPTR v) {
        v->closeWriteableValue();
    }
    static const NATIVE & accessReadableValue(const NATIVE &, const VALUEPTR v) {
        return v->getValue();
    }
};

template <class NATIVE, class VALUEPTR>
struct StaticAccessProtocol {
    static NATIVE & openWriteableValue(NATIVE & n, VALUEPTR) {
        return n;
    }
    static void closeWriteableValue(NATIVE & n, VALUEPTR) {}
    static const NATIVE & accessReadableValue(const NATIVE & n, const VALUEPTR) {
        return n;
    }
};

template <class NATIVE, class VALUEPTR>
struct NodeAccessProtocol {
    static NATIVE & openWriteableValue(NATIVE &, VALUEPTR v) {
        return v->dom::Node::nodeValueRefOpen<NATIVE>();
    }
    static void closeWriteableValue(NATIVE &, VALUEPTR v) {
        v->dom::Node::nodeValueRefClose<NATIVE>();
    }
    static const NATIVE & accessReadableValue(const NATIVE &, const VALUEPTR v) {
        return v->dom::Node::nodeValueRef<NATIVE>();
        //return v->getValue();
    }
};

template <class NATIVE, class VALUEPTR>
struct VectorValueAccessProtocol {
    static NATIVE & openWriteableValue(NATIVE &, VALUEPTR v) {
        return dynamic_cast<NATIVE &>(*v);
    }
    static void closeWriteableValue(NATIVE &, VALUEPTR v) {
    }
    static const NATIVE & accessReadableValue(const NATIVE &, const VALUEPTR v) {
        return dynamic_cast<const NATIVE &>(*v);
    }
};

class JSWrapperBase {
public:
    static JSWrapperBase * getBasePtr(JSContext *cx, JSObject *obj) {
        JSWrapperBase * myJSWrapper = static_cast<JSWrapperBase*>(JS_GetPrivate(cx,obj));
        if (!myJSWrapper) {
            JS_ReportError(cx,"JSWrapperBase::getJSWrapperPtr: internal error, binding object does not exist");
        }
        return myJSWrapper;
    }
    virtual const void * getNativeAdress() const = 0;
    virtual ~JSWrapperBase() {}; 
};


template <class NATIVE,
          class OWNERPTR=asl::Ptr<typename dom::ValueWrapper<NATIVE>::Type, dom::ThreadingModel>,
          template<class,class> class ACCESS_PROTOCOL = ValueAccessProtocol>
class JSWrapper : public JSWrapperBase {
public:
    typedef ACCESS_PROTOCOL<NATIVE, OWNERPTR> NATIVE_ACCESS_PROTOCOL;
    typedef OWNERPTR NativeValuePtr;

    static
    bool matchesClassOf(JSContext *cx, jsval theVal);

    static
    JSWrapper * getJSWrapperPtr(JSContext *cx, JSObject *obj);
    
    static
    JSWrapper & getJSWrapper(JSContext *cx, JSObject *obj);

    static void finalizeImpl(JSContext *cx, JSObject *obj); 

    virtual unsigned long length() const; 

    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, 
            jsval id, jsval *vp); 

    virtual JSBool getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, 
            jsval id, jsval *vp); 

    virtual JSBool getPropertyByNumericId(unsigned long theID, JSContext *cx, JSObject *obj, 
            jsval id, jsval *vp);

    virtual JSBool getPropertyByLiteralId(const std::string & theID, JSContext *cx, 
            JSObject *obj, jsval id, jsval *vp);

    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, 
            jsval id, jsval *vp);
    
    virtual JSBool setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, 
            jsval id, jsval *vp);

    virtual JSBool setPropertyByNumericId(unsigned long theID, JSContext *cx, JSObject *obj, 
            jsval id, jsval *vp);

    virtual JSBool setPropertyByLiteralId(const std::string & theID, JSContext *cx, 
            JSObject *obj, jsval id, jsval *vp);

    static JSFunctionSpec * Functions() {
        JSFunctionSpec * myFunctions = getMethods();
        return myFunctions;
    }
    static JSPropertySpec * Properties() {
        JSPropertySpec * myProperties = getProperties();
        return myProperties;
    }

// This should be private to enforce the usage of (scoped) WritableNative
// which is being closed, even in case of exceptions between open and close
public:
    virtual NATIVE & openNative();
    virtual void closeNative(); 

public:
    class WritableNative {
        public:
            WritableNative(JSWrapper * theWrapper) :
                _myNative(theWrapper->openNative()),
                _myWrapper(theWrapper)
            {}
            ~WritableNative() {
                _myWrapper->closeNative();
            }
            NATIVE & get() {
                return _myNative;
            }
        private:
            NATIVE & _myNative;
            JSWrapper * _myWrapper;
    };
    
    virtual const void * getNativeAdress() const {
        return &getNative();
    }
    virtual const NATIVE & getNative() const;

    virtual OWNERPTR & getOwner();
    virtual const OWNERPTR & getOwner() const;

    static JSBool
    getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
    
    static JSBool
    setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    newResolve(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp); 

    class JSClassSingleton :
        public asl::Singleton<JSClassSingleton>
    {
    public:
        friend class asl::SingletonManager;
        JSClass * getClass(const char * theName = 0) {
            if (theName) {
                _myJSClass.name = theName;
            }
            return &_myJSClass;
        }
        std::vector<JSFunctionSpec> & getMethodsRef() {
            return myMethods;
        } 
        std::vector<JSPropertySpec> & getPropertiesRef() {
            return  myProperties; 
        }
        std::vector<JSFunctionSpec> & getStaticFunctionsRef() {
            return myStaticFunctions;
        } 
        std::vector<JSPropertySpec> & getStaticPropertiesRef() {
            return myStaticProperties;
        } 
     private:
        JSClassSingleton() {
            JSClass myTempClass = {
                "<notsetyet>",                        // const char          *name;
                JSCLASS_HAS_PRIVATE |                 // uint32              flags;
                //JSCLASS_HAS_PRIVATE |               /* objects have private slot */
                //JSCLASS_NEW_ENUMERATE |             /* has JSNewEnumerateOp hook */
                JSCLASS_NEW_RESOLVE //|                 /* has JSNewResolveOp hook */
                //JSCLASS_PRIVATE_IS_NSISUPPORTS |    /* private is (nsISupports *) */
                //JSCLASS_SHARE_ALL_PROPERTIES |      /* all properties are SHARED */
                //JSCLASS_NEW_RESOLVE_GETS_START      //JSNewResolveOp gets starting
                                                      //object in prototype chain
                                                      //passed in via *objp in/out
                                                      //parameter */
                ,
                /* Mandatory non-null function pointer members. */
                NoisyAddProperty,    // JSPropertyOp        addProperty;
                NoisyDelProperty,    // JSPropertyOp        delProperty;
                JSWrapper::getProperty,         // JSPropertyOp        getProperty;
                JSWrapper::setProperty,         // JSPropertyOp        setProperty;
                NoisyEnumerate,      // JSEnumerateOp       enumerate;
                //NoisyResolve,        // JSResolveOp         resolve;
                (JSResolveOp)JSWrapper::newResolve,  // JSResolveOp         resolve;
                NoisyConvert,        // JSConvertOp         convert;
                JSWrapper::finalizeImpl,       // JSFinalizeOp        finalize;

                JSCLASS_NO_OPTIONAL_MEMBERS
                /* Optionally non-null members start here. */
                                    // JSGetObjectOps      getObjectOps;
                                    // JSCheckAccessOp     checkAccess;
                                    // JSNative            call;
                                    // JSNative            construct;
                                    // JSXDRObjectOp       xdrObject;
                                    // JSHasInstanceOp     hasInstance;
                                    // JSMarkOp            mark;
                                    // jsword              spare;
            };
            _myJSClass = myTempClass;
        }
    private:
        JSClass _myJSClass;
        std::vector<JSFunctionSpec> myMethods; 
        std::vector<JSPropertySpec> myProperties; 
        std::vector<JSFunctionSpec> myStaticFunctions; 
        std::vector<JSPropertySpec> myStaticProperties; 
    }; // end JSClassSingleton

    static JSClass * Class(const char * theName = 0); 

    static const char * ClassName(const char * theName = 0); 

    static
    JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative); 
    
    static JSObject *
    ConstructWithArgs(JSContext * cx, OWNERPTR theOwner, NATIVE * theNative, 
                uintN argc, jsval * argv);

    static JSObject *
    asJSVal(JSContext * cx, OWNERPTR theOwner, NATIVE * theNative); 

protected:
    JSWrapper(OWNERPTR theOwner, NATIVE * theNative);
    JSWrapper();

    static JSObject * initClass(JSContext *cx,
        JSObject *theGlobalObject,
        const char * theClassName,
        JSNative theConstructor,
        JSPropertySpec * theProperties,
        JSFunctionSpec * theFunctions,
        JSConstIntPropertySpec * theConstIntProperties = 0,
        JSPropertySpec * theStaticProperties = 0,
        JSFunctionSpec * theStaticFunctions = 0 );

    static bool registerFunction(std::vector<JSFunctionSpec> & theList, const char * theName, JSNative theFunction, uint8 theMinArgCount) {
//AC_INFO<<"Registering function "<<theName;
        JSFunctionSpec mySpec;
        mySpec.name = theName;
        mySpec.call = theFunction;
        mySpec.nargs = theMinArgCount;
        mySpec.flags = 0;
        mySpec.extra = 0;
        theList.push_back(mySpec);
        return true;
    }
    static bool registerProperty(std::vector<JSPropertySpec> & theList,
                                 const char * theName,
                                 JSPropertyOp theGetter,
                                 JSPropertyOp theSetter) {
//        AC_INFO<<"Registering property "<<theName;
        JSPropertySpec mySpec;
        mySpec.name = theName;
        mySpec.tinyid = static_cast<int8>(theList.size()-100);
        mySpec.getter = theGetter;
        mySpec.setter = theSetter;
        uint8 myFlags = JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED;
        if (!theSetter) {
            myFlags |= JSPROP_READONLY;
        }
        mySpec.flags = 0;
        theList.push_back(mySpec);
        return true;
    }
    static JSFunctionSpec * getMethods() {
        registerMethod(0,0,0); // appends end delimiter
        return &JSClassSingleton::get().getMethodsRef()[0];
    }
    static JSFunctionSpec * getStaticFunctions() {
        registerStaticFunction(0,0,0); // appends end delimiter
        return &JSClassSingleton::get().getStaticFunctionsRef()[0];
    }
    static JSPropertySpec * getProperties() {
        registerProperty(0,0,0); // appends end delimiter
        return &JSClassSingleton::get().getPropertiesRef()[0];
    }
 public:
    static bool registerMethod(const char * theName, JSNative theFunction, uint8 theMinArgCount) {
        return registerFunction(JSClassSingleton::get().getMethodsRef(), theName, theFunction, theMinArgCount);
    }
    static bool registerStaticFunction(const char * theName, JSNative theFunction, uint8 theMinArgCount) {
        return registerFunction(JSClassSingleton::get().getStaticFunctionsRef(), theName, theFunction, theMinArgCount);
    }
    static bool registerProperty(const char * theName,
                                 JSPropertyOp theGetter,
                                 JSPropertyOp theSetter) {
        return registerProperty(JSClassSingleton::get().getPropertiesRef(), theName, theGetter, theSetter);
    }
private:    
    OWNERPTR _myOwner;
    NATIVE * _myNative; // only used when instantiated with NATIVE_ACCESS_PROTOCOL = StaticAccessProtocol
};


template <class NATIVE, class JSWRAPPER>
struct JSClassTraitsWrapper {
    static JSClass * Class() {
        return JSWRAPPER::Class();
    }
    static NATIVE & openNativeRef(JSContext *cx, JSObject * theJSObj) {
        return JSWRAPPER::getJSWrapper(cx, theJSObj).openNative();
    }
    static void closeNativeRef(JSContext *cx, JSObject * theJSObj) {
        JSWRAPPER::getJSWrapper(cx, theJSObj).closeNative();
    }
    static typename JSWRAPPER::NativeValuePtr getNativeOwner(JSContext *cx, JSObject * theJSObj) {
        return JSWRAPPER::getJSWrapper(cx, theJSObj).getOwner();
    }
    static const NATIVE & getNativeRef(JSContext *cx, JSObject * theJSObj) {
        return JSWRAPPER::getJSWrapper(cx, theJSObj).getNative();
    }
    static bool registerMethod(const char * theName, JSNative theFunction, uint8 theMinArgCount) {
        return JSWRAPPER::registerMethod(theName, theFunction, theMinArgCount);
    }
    static bool registerProperty(const char * theName, JSPropertyOp theGetter, JSPropertyOp theSetter) {
        return JSWRAPPER::registerProperty(theName, theGetter, theSetter);
    }

    struct ScopedNativeRef {
        ScopedNativeRef(JSContext *cx, JSObject * theJSObj)
            : _myWrapper(JSWRAPPER::getJSWrapper(cx, theJSObj)),
              _myNative(_myWrapper.openNative()) {}
        ~ScopedNativeRef(){
            _myWrapper.closeNative();
        }
        NATIVE & getNative() {
            return _myNative;
        }
    private:
        typename JSWRAPPER::Base & _myWrapper;
        NATIVE & _myNative;
    };
};

DEFINE_EXCEPTION(BadArgumentException, asl::Exception);

void
checkForUndefinedArguments(const std::string & theMethodName, uintN argc, jsval *argv);

void
checkArguments(const std::string & theMethodName, uintN argc, jsval *argv, 
        unsigned theRequiredArguments);

bool
isCalledForConversion(JSContext * cx, uintN argc, jsval * argv);

}
#endif


