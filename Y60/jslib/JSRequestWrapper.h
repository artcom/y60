//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSRequestWrapper.h,v $
//   $Author: christian $
//   $Revision: 1.4 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSCURL_INCLUDED_
#define _Y60_ACXPSHELL_JSCURL_INCLUDED_

#include "JSWrapper.h"
#include "JSRequest.h"

#include <asl/string_functions.h>

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
            PROP_responseCode,
        };
        static JSPropertySpec * Properties();
        static JSConstIntPropertySpec * ConstIntProperties();
        static JSPropertySpec * StaticProperties();
        static JSFunctionSpec * StaticFunctions();


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


bool convertFrom(JSContext *cx, jsval theValue, inet::RequestPtr & theRequest);
bool convertFrom(JSContext *cx, jsval theValue, JSRequestWrapper::OWNERPTR & theRequest);
bool convertFrom(JSContext *cx, jsval theValue, JSRequestWrapper::NATIVE & theRequest);

jsval as_jsval(JSContext *cx, JSRequestWrapper::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSRequestWrapper::OWNERPTR theOwner, JSRequestWrapper::NATIVE * theSerial);

}

#endif

