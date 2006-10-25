//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSSERIAL_INCLUDED_
#define _Y60_ACXPSHELL_JSSERIAL_INCLUDED_

#include <y60/JSWrapper.h>

#include <asl/SerialDevice.h>
#include <asl/uCCP.h>

#include <asl/string_functions.h>

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

bool convertFrom(JSContext *cx, jsval theValue, JSSerial::NATIVE & theSerial);

jsval as_jsval(JSContext *cx, JSSerial::OWNERPTR theOwner);
jsval as_jsval(JSContext *cx, JSSerial::OWNERPTR theOwner, JSSerial::NATIVE * theSerial);

}

#endif

