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
//   $RCSfile: JSSocket.h,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/02/25 14:58:30 $
//
//
//=============================================================================

#ifndef _ac_jslib_JSSocket_h_
#define _ac_jslib_JSSocket_h_

#include <y60/JSWrapper.h>
#include <asl/Socket.h>


class JSSocket : public jslib::JSWrapper<inet::Socket, asl::Ptr<inet::Socket> , jslib::StaticAccessProtocol> {
        JSSocket() {}
    public:
        typedef inet::Socket NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef jslib::JSWrapper<NATIVE,OWNERPTR, jslib::StaticAccessProtocol> Base;

        JSSocket(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
        {}

        virtual ~JSSocket();

        static const char * ClassName() {
            return "Socket";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_isValid = -100,
            PROP_sendBufferSize,
            PROP_receiveBufferSize,
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

        virtual JSBool getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);
        virtual JSBool setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp);

        static JSBool
        Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

        static
        JSObject * Construct(JSContext *cx, OWNERPTR theOwner, NATIVE * theNative) {
            return Base::Construct(cx, theOwner, theNative);
        }

        static jslib::JSConstIntPropertySpec * ConstIntProperties();
        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSSocket & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSSocket &>(JSSocket::getJSWrapper(cx,obj));
        }
};

namespace jslib {
template <>
struct JSClassTraits<JSSocket::NATIVE>
    : public JSClassTraitsWrapper<JSSocket::NATIVE , JSSocket> {};

} // namespace jslib

bool convertFrom(JSContext *cx, jsval theValue, JSSocket::NATIVE & theNative);
bool convertFrom(JSContext *cx, jsval theValue, JSSocket::OWNERPTR & theNativePtr);

jsval as_jsval(JSContext *cx, JSSocket::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSSocket::OWNERPTR theOwner, JSSocket::NATIVE * theSerial);

#endif

