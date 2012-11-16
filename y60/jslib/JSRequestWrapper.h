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

#ifndef _Y60_ACXPSHELL_JSCURL_INCLUDED_
#define _Y60_ACXPSHELL_JSCURL_INCLUDED_

#include "y60_jslib_settings.h"

#include <y60/jsbase/JSWrapper.h>
#include "JSRequest.h"

#include <asl/base/string_functions.h>

namespace jslib {

class JSRequestWrapper : public JSWrapper<JSRequest, JSRequestPtr, StaticAccessProtocol> {
        JSRequestWrapper() {}
    public:
        typedef JSRequest NATIVE;
        typedef JSRequestPtr OWNERPTR;
        typedef JSWrapper<JSRequest, JSRequestPtr, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "Request";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_URL = -100,
            PROP_errorString,
            PROP_responseString,
            PROP_responseBlock,
            PROP_responseCode,
            PROP_verbose,
            PROP_verifyPeer,
            PROP_END
        };
        static JSPropertySpec * Properties();
        static JSConstIntPropertySpec * ConstIntProperties();
        static JSPropertySpec * StaticProperties();
        static JSFunctionSpec * StaticFunctions();

        virtual ~JSRequestWrapper() {
            AC_TRACE << "JSRequestWrapper::dtor called";
        }


        virtual unsigned long length() const {
            return 1;
        }

        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        JSRequestWrapper(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSRequestWrapper & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSRequestWrapper &>(JSRequestWrapper::getJSWrapper(cx,obj));
        }

    private:
};


template <>
struct JSClassTraits<JSRequestWrapper::NATIVE>
    : public JSClassTraitsWrapper<JSRequestWrapper::NATIVE, JSRequestWrapper> {};

template <>
struct JSClassTraits<inet::Request>
    : public JSClassTraitsWrapper<inet::Request, JSRequestWrapper> {};


Y60_JSLIB_DECL bool convertFrom(JSContext *cx, jsval theValue, inet::RequestPtr & theRequest);
Y60_JSLIB_DECL bool convertFrom(JSContext *cx, jsval theValue, JSRequestWrapper::OWNERPTR & theRequest);
Y60_JSLIB_DECL bool convertFrom(JSContext *cx, jsval theValue, JSRequestWrapper::NATIVE & theRequest);

Y60_JSLIB_DECL jsval as_jsval(JSContext *cx, JSRequestWrapper::OWNERPTR theOwner);
Y60_JSLIB_DECL jsval as_jsval(JSContext *cx, JSRequestWrapper::OWNERPTR theOwner, JSRequestWrapper::NATIVE * theSerial);

}

#endif

