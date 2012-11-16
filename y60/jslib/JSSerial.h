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

#ifndef _Y60_ACXPSHELL_JSSERIAL_INCLUDED_
#define _Y60_ACXPSHELL_JSSERIAL_INCLUDED_

#include "y60_jslib_settings.h"

#include <y60/jsbase/JSWrapper.h>

#include <asl/serial/SerialDevice.h>
#include <asl/serial/uCCP.h>

#include <asl/base/string_functions.h>

namespace jslib {

class JSSerial : public JSWrapper<asl::SerialDevice, asl::Ptr<asl::SerialDevice>, StaticAccessProtocol>
{
        JSSerial() : _myUCCP(0) {}
    public:
        typedef asl::SerialDevice NATIVE;
        typedef asl::Ptr<NATIVE> OWNERPTR;
        typedef JSWrapper<NATIVE,OWNERPTR,StaticAccessProtocol> Base;

        static const char * ClassName() {
            return "SerialDevice";
        }
        static JSFunctionSpec * Functions();

        enum PropertyNumbers {
            PROP_isOpen = -100,
            PROP_status,
            PROP_NO_PARITY,
            PROP_EVEN_PARITY,
            PROP_ODD_PARITY,
            PROP_NO_CHECKING,
            PROP_CRC8_CHECKING,
            PROP_CHECKSUM_CHECKING,
            PROP_STATUS_CTS,
            PROP_STATUS_DSR,
            PROP_STATUS_RI,
            PROP_STATUS_CD,
            PROP_STATUS_DTR,
            PROP_STATUS_RTS,
        };
        static JSPropertySpec * Properties();

        static JSFunctionSpec * StaticFunctions();
        static JSPropertySpec * StaticProperties();

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

        JSSerial(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative), _myUCCP(theNative)
        {}

        static JSConstIntPropertySpec * ConstIntProperties();

        static JSObject * initClass(JSContext *cx, JSObject *theGlobalObject);

        static JSSerial & getObject(JSContext *cx, JSObject * obj) {
            return dynamic_cast<JSSerial &>(JSSerial::getJSWrapper(cx,obj));
        }

        asl::uCCP & getUCCP() {
            return _myUCCP;
        }

    private:
        asl::uCCP _myUCCP;
};

template <>
struct JSClassTraits<asl::SerialDevice> : public JSClassTraitsWrapper<asl::SerialDevice, JSSerial> {};

Y60_JSLIB_DECL bool convertFrom(JSContext *cx, jsval theValue, JSSerial::NATIVE & theSerial);

Y60_JSLIB_DECL jsval as_jsval(JSContext *cx, JSSerial::OWNERPTR theOwner);
Y60_JSLIB_DECL jsval as_jsval(JSContext *cx, JSSerial::OWNERPTR theOwner, JSSerial::NATIVE * theSerial);

}

#endif

