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

#ifndef _Y60_JSHermiteSpline_H_
#define _Y60_JSHermiteSpline_H_

#include "y60_jslib_settings.h"

#include <y60/jsbase/JSWrapper.h>

#include <asl/math/Vector234.h>
#include <asl/math/Spline.h>
#include <asl/dom/Value.h>

namespace jslib {

class JSHermiteSpline : public JSWrapper<asl::Hermite, asl::Ptr<asl::Hermite>, StaticAccessProtocol>
{
    JSHermiteSpline() {}

public:
    typedef asl::Hermite NATIVE;
    typedef asl::Ptr<NATIVE> OWNERPTR;
    typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

    static const char * ClassName() {
        return "HermiteSpline";
    };

    enum PropertyNumbers {
        PROP_xFirst = -100,
        PROP_xLast,
        PROP_yFirst,
        PROP_yLast,
        PROP_dim
    };

    static JSFunctionSpec * Functions();
    static JSPropertySpec * Properties();
    static JSConstIntPropertySpec * ConstIntProperties();
    static JSPropertySpec * StaticProperties();
    static JSFunctionSpec * StaticFunctions();

    virtual unsigned long length() const {
        return 1;
    }

    // getproperty handling
    virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    // setproperty handling
    virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

    static JSBool
    Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

    static
    JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
        return Base::Construct(cx, theOwner, theNative);
    }

    JSHermiteSpline(OWNERPTR theOwner, NATIVE * theNative) : Base(theOwner, theNative) {
    }

    static JSObject * initClass(JSContext *cx, JSObject * theGlobalObject);
};

Y60_JSLIB_DECL bool convertFrom(JSContext * cx, jsval theValue, JSHermiteSpline::NATIVE *& theBSpline);
Y60_JSLIB_DECL bool convertFrom(JSContext * cx, jsval theValue, JSHermiteSpline::NATIVE & theBSpline);

Y60_JSLIB_DECL jsval as_jsval(JSContext * cx, JSHermiteSpline::OWNERPTR theOwner);
Y60_JSLIB_DECL jsval as_jsval(JSContext * cx, JSHermiteSpline::OWNERPTR theOwner, JSHermiteSpline::NATIVE * theBSpline);

} // namespace

#endif
