//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSTESTSERVER_INCLUDED_
#define _Y60_ACXPSHELL_JSTESTSERVER_INCLUDED_

#include "JSWrapper.h"
#include <y60/TestServer.h>

#include <asl/string_functions.h>

namespace jslib {

class JSTestServer : public JSWrapper<asl::ConduitAcceptor<asl::TCPPolicy>, asl::Ptr<asl::ConduitAcceptor<asl::TCPPolicy> >, StaticAccessProtocol> {
        JSTestServer() {}
    public:
        typedef asl::ConduitAcceptor<asl::TCPPolicy> NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE, OWNERPTR, StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "TestServer";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_END = -100
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

        JSTestServer(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSTestServer & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSTestServer &>(JSTestServer::getJSWrapper(cx,obj));
        }

    private:
};


template <>
struct JSClassTraits<JSTestServer::NATIVE>
    : public JSClassTraitsWrapper<JSTestServer::NATIVE, JSTestServer> {};

bool convertFrom(JSContext *cx, jsval theValue, JSTestServer::OWNERPTR & theRequest);

jsval as_jsval(JSContext *cx, JSTestServer::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSTestServer::OWNERPTR theOwner, JSTestServer::NATIVE * theSerial);

}

#endif


