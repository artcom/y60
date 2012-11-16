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

#ifndef _ac_jslib_JSSocket_h_
#define _ac_jslib_JSSocket_h_

#include "y60_net_settings.h"

#include <y60/jsbase/JSWrapper.h>
#include <asl/net/Socket.h>

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
            PROP_isConnected,
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

Y60_NET_DECL bool convertFrom(JSContext *cx, jsval theValue, JSSocket::NATIVE & theNative);
Y60_NET_DECL bool convertFrom(JSContext *cx, jsval theValue, JSSocket::OWNERPTR & theNativePtr);

Y60_NET_DECL jsval as_jsval(JSContext *cx, JSSocket::OWNERPTR theOwner);
Y60_NET_DECL jsval as_jsval(JSContext *cx, JSSocket::OWNERPTR theOwner, JSSocket::NATIVE * theSerial);

#endif

