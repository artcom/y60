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

#ifndef _Y60_ACXPSHELL_ARGUMENTHOLDER_H_INCLUDED_
#define _Y60_ACXPSHELL_ARGUMENTHOLDER_H_INCLUDED_

#include "y60_jsbase_settings.h"


#include "Documentation.h"
#include "JScppUtils.h"

#ifdef USE_LEGACY_SPIDERMONKEY
#include <js/spidermonkey/jspubtd.h>
#include <js/spidermonkey/jsapi.h>
#include <js/spidermonkey/jscntxt.h>
#include <js/spidermonkey/jsgc.h>
#include <js/spidermonkey/jslock.h>
#include <js/spidermonkey/jsnum.h>
#else
#include <js/jspubtd.h>
#include <js/jsapi.h>
#include <js/jscntxt.h>
#include <js/jsgc.h>
#include <js/jslock.h>
#include <js/jsnum.h>
#endif

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


