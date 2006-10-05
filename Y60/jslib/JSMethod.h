//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSMETHOD_H_INCLUDED_
#define _Y60_ACXPSHELL_JSMETHOD_H_INCLUDED_

#include "Documentation.h"
#include "JScppUtils.h"

#include <js/jspubtd.h>
#include <js/jsapi.h>
#include <js/jscntxt.h>
#include <js/jsgc.h>
#include <js/jslock.h>
#include <js/jsnum.h>
#include <dom/Nodes.h>
#include <asl/Singleton.h>

#include <set>

namespace jslib {

template<class T>
struct Argument {
    typedef T PlainType;
    typedef T FullType;
};

template<class T>
struct Argument<T&> {
    typedef T PlainType;
    typedef T & FullType;
};

template<class T>
struct Argument<const T &> {
    typedef T PlainType;
    typedef const T & FullType;
};

#if 0
struct Argument<const dom::Node &> {
    typedef dom::Node * const PlainType;
    typedef const dom::Node & FullType;
};
#endif

template<class T>
struct Argument<const T> {
    typedef T PlainType;
    typedef const T FullType;
};

struct NoArgument {};

struct NoResult {
    template <class T> NoResult(T) {}
};

template <>
struct Argument<NoResult> {
    typedef void PlainType;
    typedef NoResult FullType;
};

DEFINE_EXCEPTION(ArgumentConversionFailed, asl::Exception);

template <class T, unsigned N>
struct ArgumentHolder {
    typedef T ARG_TYPE;
    typedef typename Argument<T>::PlainType PLAIN_TYPE;

    ArgumentHolder(JSCallArgs & theArgs) {
        if(!convertFrom(theArgs.cx, theArgs.argv[N], _myArg)) {
            throw ArgumentConversionFailed(JUST_FILE_LINE);
        }
    }
    PLAIN_TYPE & getArg() {
        return _myArg;
    };
private:
    PLAIN_TYPE _myArg;
};

template <unsigned N>
struct ArgumentHolder<NoArgument,N> {
    ArgumentHolder(JSCallArgs & theArgs) {}
    typedef void ARG_TYPE;
    enum { n = N };
    enum { exists = false };
    NoArgument & getArg() {
        static NoArgument _ourNoArgument;
        return _ourNoArgument;
    };
};
#if 1
extern bool convertFrom(JSContext *cx, jsval theValue, const dom::Node * & theNode);
template <unsigned N>
struct ArgumentHolder<const dom::Node &, N> {
    typedef const dom::Node & ARG_TYPE;
    typedef const dom::Node & PLAIN_TYPE;

    ArgumentHolder(JSCallArgs & theArgs) {
        if(!convertFrom(theArgs.cx, theArgs.argv[N], _myArg)) {
            throw ArgumentConversionFailed(JUST_FILE_LINE);
        }
    }
	const dom::Node & getArg() const {
        return *_myArg;
    };
private:
	dom::Node const * _myArg; // pointer to const Node, same as const dom::Node * _myArg
};
#endif

template <class T>
struct ResultConverter {
    static void
    storeResult(JSCallArgs & theJSArgs, const T & theResult) {
        *theJSArgs.rval = as_jsval(theJSArgs.cx, theResult);
    }
};

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
        _myArgHolder0(theJSArgs),
        _myArgHolder1(theJSArgs),
        _myArgHolder2(theJSArgs),
        _myArgHolder3(theJSArgs),
        _myArgHolder4(theJSArgs),
        _myArgHolder5(theJSArgs),
        _myArgHolder6(theJSArgs),
        _myArgHolder7(theJSArgs)
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
    ArgumentHolder<ARG0,0> _myArgHolder0;
    ArgumentHolder<ARG1,1> _myArgHolder1;
    ArgumentHolder<ARG2,2> _myArgHolder2;
    ArgumentHolder<ARG3,3> _myArgHolder3;
    ArgumentHolder<ARG4,4> _myArgHolder4;
    ArgumentHolder<ARG5,5> _myArgHolder5;
    ArgumentHolder<ARG6,6> _myArgHolder6;
    ArgumentHolder<ARG7,7> _myArgHolder7;
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
        _myArgHolder0(theJSArgs),
        _myArgHolder1(theJSArgs),
        _myArgHolder2(theJSArgs),
        _myArgHolder3(theJSArgs),
        _myArgHolder4(theJSArgs),
        _myArgHolder5(theJSArgs),
        _myArgHolder6(theJSArgs),
        _myArgHolder7(theJSArgs)
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
    ArgumentHolder<ARG0,0> _myArgHolder0;
    ArgumentHolder<ARG1,1> _myArgHolder1;
    ArgumentHolder<ARG2,2> _myArgHolder2;
    ArgumentHolder<ARG3,3> _myArgHolder3;
    ArgumentHolder<ARG4,4> _myArgHolder4;
    ArgumentHolder<ARG5,5> _myArgHolder5;
    ArgumentHolder<ARG6,6> _myArgHolder6;
    ArgumentHolder<ARG7,7> _myArgHolder7;
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
            JSRuntime * myRuntime=JS_GetRuntime(cx);
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

}
#endif


