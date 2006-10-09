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

template <class NATIVE,
          class OWNERPTR=asl::Ptr<typename dom::ValueWrapper<NATIVE>::Type, dom::ThreadingModel>,
          template<class,class> class ACCESS_PROTOCOL = ValueAccessProtocol>
class JSWrapper {
public:
    typedef ACCESS_PROTOCOL<NATIVE, OWNERPTR> NATIVE_ACCESS_PROTOCOL;
    typedef OWNERPTR NativeValuePtr;

    static
    bool matchesClassOf(JSContext *cx, jsval theVal) {
        JSObject * myObj;
        if (JSVAL_IS_VOID(theVal)) {
            JS_ReportError(cx,"matchesClassOf: passed 'undefined' as object");
            return false;
        }
        if (!JSVAL_IS_OBJECT(theVal) || !JS_ValueToObject(cx, theVal, &myObj)) {
            return false;
        }
        return (JSA_GetClass(cx,myObj) == Class());
    }

    static
    JSWrapper * getJSWrapperPtr(JSContext *cx, JSObject *obj) {
        if (JSA_GetClass(cx,obj) != Class()) {
            JS_ReportError(cx,"JSWrapper::getJSWrapperPtr: class type %s expected",ClassName());
            return 0;
        }
        JSWrapper * myJSWrapper = static_cast<JSWrapper*>(JS_GetPrivate(cx,obj));
        if (!myJSWrapper) {
            JS_ReportError(cx,"JSWrapper::getJSWrapperPtr: internal error, binding object does not exist");
        }
        return myJSWrapper;
    }
    static
    JSWrapper & getJSWrapper(JSContext *cx, JSObject *obj) {
        JSWrapper * myJSWrapper = getJSWrapperPtr(cx,obj);
        if (!myJSWrapper) {
            JS_ReportError(cx,"JSWrapper::getJSWrapper: internal error, binding object does not exist");
            static JSWrapper myDummy;
            return myDummy;
        }
        return *myJSWrapper;
    }
    static void finalizeImpl(JSContext *cx, JSObject *obj) {
        if (JS_GetPrivate(cx,obj) != Class()) {
            JSWrapper * myImpl = static_cast<JSWrapper*>(JS_GetPrivate(cx,obj));
            delete myImpl;
        }
    }

    virtual unsigned long length() const {
        return 0;
    }

    // getproperty handling
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        JS_ReportError(cx,"JSWrapper::getPropertySwitch: index %d out of range", theID);
        return JS_FALSE;
    };
    virtual JSBool getPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        JS_ReportError(cx,"JSWrapper::getPropertyIndex: index %d out of range", theIndex);
        return JS_FALSE;
    };

    virtual JSBool getPropertyByNumericId(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        if ((theID >= 0) && (theID < length())) {
            return getPropertyIndex(theID, cx, obj, id, vp);
        }
        return getPropertySwitch(theID, cx, obj, id, vp);
    };
    virtual JSBool getPropertyByLiteralId(const std::string & theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        return JS_TRUE;
    };

    // setproperty handling
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        JS_ReportError(cx,"JSWrapper::setPropertySwitch: index %d out of range", theID);
        return JS_FALSE;
    };
    virtual JSBool setPropertyIndex(unsigned long theIndex, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        JS_ReportError(cx,"JSWrapper::setPropertyIndex: index %d out of range", theIndex);
        return JS_FALSE;
    };

    virtual JSBool setPropertyByNumericId(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        if ((theID >= 0) && (theID < length())) {
            return setPropertyIndex(theID, cx, obj, id, vp);
        }
        return setPropertySwitch(theID, cx, obj, id, vp);
    };
    virtual JSBool setPropertyByLiteralId(const std::string & theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        return JS_TRUE;
    };

// This should be private to enforce the usage of (scoped) WritableNative
// which is being closed, even in case of exceptions between open and close
public:
    virtual NATIVE & openNative() {
        return NATIVE_ACCESS_PROTOCOL::openWriteableValue(*_myNative, _myOwner);
    }
    virtual void closeNative() {
        return NATIVE_ACCESS_PROTOCOL::closeWriteableValue(*_myNative, _myOwner);
    }

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
    /*
    virtual NATIVE & getNative() {
        return *_myNative;
    }
    */
    virtual const NATIVE & getNative() const {
        return NATIVE_ACCESS_PROTOCOL::accessReadableValue(*_myNative, _myOwner);
    }

    virtual OWNERPTR & getOwner() {
        return _myOwner;
    }
    virtual const OWNERPTR & getOwner() const {
        return _myOwner;
    }

    static JSBool
    getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
    {
        try {
            IF_NOISY(printParams("JSWrapper::getProperty",cx,obj,id,vp));
            JSWrapper & myJSWrapper = getJSWrapper(cx,obj);
            if (JSVAL_IS_INT(id)) {
                int myIndex = JSVAL_TO_INT(id);
                return myJSWrapper.getPropertyByNumericId(myIndex, cx, obj, id, vp);
            } else {
                JSString * myJSStr = JS_ValueToString(cx, id);
                std::string myProperty = JS_GetStringBytes(myJSStr);
                return myJSWrapper.getPropertyByLiteralId(myProperty, cx, obj, id, vp);
            }
        } HANDLE_CPP_EXCEPTION;
    }
    static JSBool
    setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        try {
            if (JS_GetPrivate(cx, obj) != Class()) {
                IF_NOISY(printParams("JSWrapper::getProperty",cx,obj,id,vp));
                JSWrapper & myJSWrapper = getJSWrapper(cx,obj);
                if (JSVAL_IS_INT(id)) {
                    int myIndex = JSVAL_TO_INT(id);
                    return myJSWrapper.setPropertyByNumericId(myIndex, cx, obj, id, vp);
                } else {
                    JSString * myJSStr = JS_ValueToString(cx, id);
                    std::string myProperty = JS_GetStringBytes(myJSStr);
                    return myJSWrapper.setPropertyByLiteralId(myProperty, cx, obj, id, vp);
                }
            }
            return JS_TRUE;
        } HANDLE_CPP_EXCEPTION;
    }
    static JSBool
    newResolve(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp) {
        *objp = 0;
        return JS_TRUE;
    }

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
    }; // end JSClassSingleton

    static JSClass * Class(const char * theName = 0) {
        return JSClassSingleton::get().getClass(theName);
    }

    static const char * ClassName(const char * theName = 0) {
        return JSClassSingleton::get().getClass(theName)->name;
    }

    static
    JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
         JSObject * myNewObj = JS_ConstructObject(cx, Class(), 0, 0);
         JSWrapper & myJSWrapper = getJSWrapper(cx,myNewObj);
         myJSWrapper._myOwner = theOwner;
         myJSWrapper._myNative = theNative;
         return myNewObj;
    }
    static JSObject *
    ConstructWithArgs(JSContext * cx, OWNERPTR theOwner, NATIVE * theNative, 
                uintN argc, jsval * argv)
    {
        JSObject * myNewObj = JS_ConstructObjectWithArguments(cx, Class(),
                0, 0, argc, argv);
        JSWrapper & myJSWrapper = getJSWrapper(cx,myNewObj);
        myJSWrapper._myOwner = theOwner;
        myJSWrapper._myNative = theNative;

        return myNewObj;
    }

    static JSObject *
    asJSVal(JSContext * cx, OWNERPTR theOwner, NATIVE * theNative) {
        jsval myArg = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "CALLED_FROM_AS_JSVAL"));
        JSObject * myReturnObject = ConstructWithArgs(cx, theOwner, & ( * theNative),
                1, & myArg);
        return myReturnObject;
    }



protected:
    JSWrapper(OWNERPTR theOwner, NATIVE * theNative)
        : _myOwner(theOwner), _myNative(theNative)
    {
    }
    JSWrapper()
        : _myNative(0), _myOwner(0)
    {
    }
    virtual ~JSWrapper() {
    }

    static JSObject * initClass(JSContext *cx,
        JSObject *theGlobalObject,
        const char * theClassName,
        JSNative theConstructor,
        JSPropertySpec * theProperties,
        JSFunctionSpec * theFunctions,
        JSConstIntPropertySpec * theConstIntProperties = 0,
        JSPropertySpec * theStaticProperties = 0,
        JSFunctionSpec * theStaticFunctions = 0 )
    {
        //createClassDocumentation(theClassName, theProperties, theFunctions, theConstIntProperties, theStaticProperties, theStaticFunctions);

        JSObject * myProtoObj = JS_InitClass(cx, theGlobalObject, NULL, Class(theClassName),
                /* native constructor function and min arg count */
                theConstructor, 0,
                /* prototype object properties and methods -- these
                will be "inherited" by all instances through
                delegation up the instance's prototype link. */
                theProperties, theFunctions,

                /* class constructor properties and methods */
                //static_props, static_methods
                theStaticProperties, theStaticFunctions
                );

        if (theConstIntProperties) {
            jsval myConstructorFuncObjVal;
            if (JS_GetProperty(cx, theGlobalObject, theClassName, &myConstructorFuncObjVal)) {
                JSObject * myConstructorFuncObj = JSVAL_TO_OBJECT(myConstructorFuncObjVal);
                JSA_DefineConstInts(cx, myConstructorFuncObj, theConstIntProperties);
            } else {
                AC_ERROR << "initClass: constructor function object not found, could not initialize static members"<<std::endl;
            }
        }

        // Initialize the private pointer to class pointer to mark this class as prototype
        JS_SetPrivate(cx, myProtoObj, Class(theClassName));

        return myProtoObj;
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

inline void
checkForUndefinedArguments(const std::string & theMethodName, uintN argc, jsval *argv) {
    for (unsigned i = 0; i < argc; ++i) {
        if (JSVAL_IS_VOID(argv[i])) {
            throw BadArgumentException(theMethodName + ": Argument " + asl::as_string(i) + " is undefined.", PLUS_FILE_LINE);
        }
    }
}

inline void
checkArguments(const std::string & theMethodName, uintN argc, jsval *argv, unsigned theRequiredArguments) {
    if (argc != theRequiredArguments) {
        throw BadArgumentException(theMethodName + ": Wrong number of arguments. Got " +
            asl::as_string(argc) + ", expected " + asl::as_string(theRequiredArguments) + ".", PLUS_FILE_LINE);
    }

    checkForUndefinedArguments(theMethodName, argc, argv);
}

inline bool
isCalledForConversion(JSContext * cx, uintN argc, jsval * argv) {
    if (JSVAL_IS_STRING(argv[0])) {
        JSString * myJSString = JSVAL_TO_STRING(argv[0]);
        if (!strcmp(JS_GetStringBytes(myJSString), "CALLED_FROM_AS_JSVAL")) {
            JS_RemoveRoot(cx,&myJSString);
            return true;
        }
    }
    return false;
}

}
#endif


