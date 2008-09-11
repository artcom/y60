//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_ARGUMENTHOLDER_H_INCLUDED_
#define _Y60_ACXPSHELL_ARGUMENTHOLDER_H_INCLUDED_

#include "Documentation.h"
#include "JScppUtils.h"
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

template <class T>
struct ArgumentHolder {
    typedef T ARG_TYPE;
    typedef typename Argument<T>::PlainType PLAIN_TYPE;

    ArgumentHolder(JSCallArgs & theArgs, int n);
    PLAIN_TYPE & getArg(); 

private:
    PLAIN_TYPE _myArg;
};

template <>
struct ArgumentHolder<NoArgument> {
    ArgumentHolder(JSCallArgs & theArgs, int n) {}
    typedef void ARG_TYPE;
    enum { exists = false };
    NoArgument & getArg() {
        static NoArgument _ourNoArgument;
        return _ourNoArgument;
    };
};

template <class T>
struct ResultConverter {
    static void
    storeResult(JSCallArgs & theJSArgs, const T & theResult);
};

}

#endif


