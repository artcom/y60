//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_jslib_JSTCPServer_h_
#define _ac_jslib_JSTCPServer_h_

#include <y60/jsbase/JSWrapper.h>
#include <asl/net/TCPServer.h>


class JSTCPServer : public jslib::JSWrapper<inet::TCPServer, asl::Ptr<inet::TCPServer> , jslib::StaticAccessProtocol> {
        JSTCPServer() {}
    public:
        typedef inet::TCPServer NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSTCPServer(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        ~JSTCPServer();

        static const char * ClassName() {
            return "TCPServer";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_isValid = -100,
            PROP_UDP,
            PROP_TCPCLIENT,
            PROP_TCPSERVER
        };
        static JSPropertySpec * Properties();

        static JSPropertySpec * StaticProperties();
        static JSFunctionSpec * StaticFunctions();

        virtual unsigned long length() const {
            return 1;
        }

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        static jslib::JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSTCPServer & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSTCPServer &>(JSTCPServer::getJSWrapper(cx,obj));
        }
};

namespace jslib {
template <>
struct JSClassTraits<JSTCPServer::NATIVE>
    : public JSClassTraitsWrapper<JSTCPServer::NATIVE , JSTCPServer> {};

} // namespace jslib

bool convertFrom(JSContext *cx, jsval theValue, JSTCPServer::NATIVE & theTCPServer);

jsval as_jsval(JSContext *cx, JSTCPServer::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSTCPServer::OWNERPTR theOwner, JSTCPServer::NATIVE * theTCPServer);

#endif

