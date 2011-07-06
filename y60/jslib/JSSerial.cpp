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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "JSSerial.h"

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSBlock.h>
#include <y60/jsbase/JSWrapper.impl>

#include <asl/serial/SerialDeviceFactory.h>
#include <asl/serial/DebugPort.h>
#include <asl/math/numeric_functions.h>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<asl::SerialDevice, asl::Ptr<asl::SerialDevice>, StaticAccessProtocol>;

const unsigned DEFAULT_BAUD_RATE    = 9600;
const unsigned DEFAULT_BITS         = 8;
const unsigned DEFAULT_PARITY       = unsigned(SerialDevice::NO_PARITY);
const unsigned DEFAULT_STOP_BITS    = 1;
const bool     DEFAULT_HW_HANDSHAKE = false;

const size_t READ_BUFFER_SIZE     = 1024;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Prints the comport device name.");
    DOC_END;
    std::string myStringRep = JSSerial::getJSWrapper(cx,obj).getNative().getDeviceName();
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}


static JSBool
open(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Opens the serial interface");
    DOC_PARAM_OPT("theBaudRate", "Baud rate", DOC_TYPE_INTEGER, DEFAULT_BAUD_RATE);
    DOC_PARAM_OPT("theDataBits", "Data bits", DOC_TYPE_INTEGER, DEFAULT_BITS);
    DOC_PARAM_OPT("theParity", "Parity",    DOC_TYPE_ENUMERATION, DEFAULT_PARITY);
    DOC_PARAM_OPT("theStopBits", "Stop Bits", DOC_TYPE_INTEGER, DEFAULT_STOP_BITS);
    DOC_PARAM_OPT("theHandshake", "Hardware handshake", DOC_TYPE_BOOLEAN, DEFAULT_HW_HANDSHAKE);
    DOC_PARAM_OPT("theMinBytesPerRead", "minimum bytes per read", DOC_TYPE_INTEGER, 0);
    DOC_PARAM_OPT("theTimeout", "timeout in ms", DOC_TYPE_INTEGER, 0);
    DOC_END;

    try {
        unsigned myBaudRate    = DEFAULT_BAUD_RATE;
        unsigned myBits        = DEFAULT_BITS;
        unsigned myParity      = DEFAULT_PARITY;
        unsigned myStopBits    = DEFAULT_STOP_BITS;
        bool     myHwHandshake = DEFAULT_HW_HANDSHAKE;
        int     myMinBytesPerRead = 0;
        int     myTimeOut = 0;

        for (unsigned i = 0; i < argc; ++i) {
            if (JSVAL_IS_VOID(argv[i])) {
                JS_ReportError(cx, "JSSerial::open(): Argument #%d is undefined", i + 1);
                return JS_FALSE;
            }
        }

        if (argc > 0) {
            if (!convertFrom(cx, argv[0], myBaudRate)) {
                JS_ReportError(cx, "JSSerial::open(): argument #1 must be an integer (Baud Rate)");
                return JS_FALSE;
            }
        }
        if (argc > 1) {
            if (!convertFrom(cx, argv[1], myBits)) {
                JS_ReportError(cx, "JSSerial::open(): argument #2 must be an integer (Data Bits)");
                return JS_FALSE;
            }
        }
        if (argc > 2) {
            if (!convertFrom(cx, argv[2], myParity)) {
                JS_ReportError(cx, "JSSerial::open(): argument #3 must be an integer (Parity Mode)");
                return JS_FALSE;
            }
            if (myParity > 2) {
                JS_ReportError(cx, "JSSerial::open(): Illegal parity mode %d, "
                                   "must be between NO_PARITY, EVEN_PARITY or ODD_PARITY", myParity);
                return JS_FALSE;
            }
        }
        if (argc > 3) {
            if (!convertFrom(cx, argv[3], myStopBits)) {
                JS_ReportError(cx, "JSSerial::open(): argument #4 must be an integer (Stop Bits)");
                return JS_FALSE;
            }
        }
        if (argc > 4) {
            if (!convertFrom(cx, argv[4], myHwHandshake)) {
                JS_ReportError(cx, "JSSerial::open(): argument #5 must be a boolean (Hardware Handshake)");
                return JS_FALSE;
            }
        }
        if (argc > 5) {
            if (!convertFrom(cx, argv[5], myMinBytesPerRead)) {
                JS_ReportError(cx, "JSSerial::open(): argument #6 must be an integer (MinBytesPerRead)");
                return JS_FALSE;
            }
        }
        if (argc > 6) {
            if (!convertFrom(cx, argv[6], myTimeOut)) {
                JS_ReportError(cx, "JSSerial::open(): argument #7 must be a integer (Timeout)");
                return JS_FALSE;
            }
        }
         if (argc > 7) {
            JS_ReportError(cx, "JSSerial::open(): Wrong number of arguments, between 0 and 5 expected.");
            return JS_FALSE;
        }

        JSSerial::getJSWrapper(cx,obj).openNative().open(myBaudRate, myBits,
            SerialDevice::ParityMode(SerialDevice::ParityModeEnum(myParity)),
            myStopBits, myHwHandshake, myMinBytesPerRead, myTimeOut);
        JSSerial::getJSWrapper(cx,obj).closeNative();

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Closes the serial device");
    DOC_END;
    return Method<JSSerial::NATIVE>::call(&JSSerial::NATIVE::close,cx,obj,argc,argv,rval);
}

static JSBool
flush(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Flushes IO buffers.");
    DOC_END;
    return Method<JSSerial::NATIVE>::call(&JSSerial::NATIVE::flush,cx,obj,argc,argv,rval);
}

static JSBool
setPacketFormat(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets the format for packets send via the serial device");
    DOC_PARAM("theStartByte", "Start byte", DOC_TYPE_INTEGER);
    DOC_PARAM("theEndByte", "End byte", DOC_TYPE_INTEGER);
    DOC_PARAM("theSize", "Payload size (may be zero for variable sized payloads.)", DOC_TYPE_INTEGER);
    DOC_PARAM("theErrorChecking", "Error checking type", DOC_TYPE_ENUMERATION);
    DOC_PARAM_OPT("theUseLengthByteFlag", "Use the payloads first byte as length field on variable packet size", DOC_TYPE_BOOLEAN, true);
    DOC_END;
    try {
        unsigned char myStartByte;
        unsigned char myEndByte;
        unsigned      mySize;
        unsigned      myErrorChecking;
        bool          myUseLengthByteFlag = true;

        for (unsigned i = 0; i < argc; ++i) {
            if (JSVAL_IS_VOID(argv[i])) {
                JS_ReportError(cx, "JSSerial::setPacketFormat(): Argument #%d is undefined", i + 1);
                return JS_FALSE;
            }
        }

        if (argc < 4) {
            JS_ReportError(cx, "JSSerial::setPacketFormat(): expected at least 4 arguments, got %d. "
                               "Usage: (StartByte, EndByte, Size, Errorchecking)", argc);
            return JS_FALSE;
        }

        uCCP & myUCCP = JSSerial::getObject(cx, obj).getUCCP();

        if (!convertFrom(cx, argv[0], myStartByte)) {
            JS_ReportError(cx, "JSSerial::setPacketFormat(): argument #1 must be an integer (Start Byte)");
            return JS_FALSE;
        }
        if (!convertFrom(cx, argv[1], myEndByte)) {
            JS_ReportError(cx, "JSSerial::setPacketFormat(): argument #2 must be an integer (End Byte)");
            return JS_FALSE;
        }

        myUCCP.setFrame(myStartByte, myEndByte);

        if (!convertFrom(cx, argv[2], mySize)) {
            JS_ReportError(cx, "JSSerial::setPacketFormat(): argument #3 must be an integer (payload size)");
            return JS_FALSE;
        }

        if (mySize == 0) {
            if (argc == 5) {
                if (!convertFrom(cx, argv[4], myUseLengthByteFlag)) {
                    JS_ReportError(cx, "JSSerial::setPacketFormat(): argument #5 must be a boolean (use length byte flag)");
                    return JS_FALSE;
                }
            }
            myUCCP.setVariablePayloadSize(myUseLengthByteFlag);
        } else {
            myUCCP.setFixedPayloadSize(mySize);
        }

        if (!convertFrom(cx, argv[3], myErrorChecking)) {
            JS_ReportError(cx, "JSSerial::setPacketFormat(): argument #4 must be an integer (errorchecking)");
            return JS_FALSE;
        }

        if (myErrorChecking > 2) {
            JS_ReportError(cx, "JSSerial::setPacketFormat(): Invalid error checking format");
            return JS_FALSE;
        }

        myUCCP.setErrorChecking(uCCP::ErrorChecking(myErrorChecking));

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
read(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Reads text (utf-8) data from serial device");
    DOC_PARAM_OPT("theSize", "Bytes to read", DOC_TYPE_INTEGER, READ_BUFFER_SIZE);
    DOC_RVAL("Bufferdata", DOC_TYPE_STRING);
    DOC_END;
    try {
        if (argc > 1) {
            JS_ReportError(cx, "JSSerial::read(): Wrong number of arguments, expected one or none, got %d.", argc);
            return JS_FALSE;
        }

        size_t myBytesWanted = READ_BUFFER_SIZE;
        if (argc == 1) {
             if (!convertFrom(cx, argv[0], myBytesWanted)) {
                JS_ReportError(cx, "JSSerial::read(): Argument #1 must be a unsigned (bytes to read)");
                return JS_FALSE;
            }
        }


        string myResult;
        char myBuffer[READ_BUFFER_SIZE];
        size_t myBytesRead = 0;
        do {
            myBytesRead = asl::minimum(myBytesWanted - myResult.size(), READ_BUFFER_SIZE);
            JSSerial::getJSWrapper(cx,obj).openNative().read(myBuffer, myBytesRead);
            JSSerial::getJSWrapper(cx,obj).closeNative();
            myResult.append(myBuffer, myBytesRead );
        } while (myBytesRead == READ_BUFFER_SIZE && // true if more data is waiting to be read
                 myResult.size() < myBytesWanted);  // true if we have more space in our myResult buffer

        *rval = as_jsval(cx, myResult);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
readBlock(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Reads binary data from serial device");
    DOC_PARAM_OPT("theSize", "Bytes to read", DOC_TYPE_INTEGER, READ_BUFFER_SIZE);
    DOC_RVAL("Bufferdata", DOC_TYPE_BLOCK);
    DOC_END;
    try {
        if (argc > 1) {
            JS_ReportError(cx, "JSSerial::readBlock(): Wrong number of arguments, expected one or none, got %d.", argc);
            return JS_FALSE;
        }

        size_t myBytesWanted = READ_BUFFER_SIZE;
        if (argc == 1) {
             if (!convertFrom(cx, argv[0], myBytesWanted)) {
                JS_ReportError(cx, "JSSerial::readBlock(): Argument #1 must be a unsigned (bytes to read)");
                return JS_FALSE;
            }
        }


        asl::Ptr<asl::Block> myResult(new Block());
        char myBuffer[READ_BUFFER_SIZE];
        size_t myBytesRead = 0;
        do {
            myBytesRead = asl::minimum(myBytesWanted - myResult->size(), READ_BUFFER_SIZE);
            JSSerial::getJSWrapper(cx,obj).openNative().read(myBuffer, myBytesRead);
            JSSerial::getJSWrapper(cx,obj).closeNative();
            myResult->append(myBuffer, myBytesRead );
        } while (myBytesRead == READ_BUFFER_SIZE && // true if more data is waiting to be read
                 myResult->size() < myBytesWanted);  // true if we have more space in our myResult buffer

        *rval = as_jsval(cx, myResult);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
static JSBool
writeBlock(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Writes data to the serial device");
    DOC_PARAM("theData", "Bytes to write", DOC_TYPE_BLOCK);
    DOC_END;
    try {
        if (argc != 1) {
            JS_ReportError(cx, "JSSerial::write(): Wrong number of arguments, "
                               "expected one (Bytes to write), got %d.", argc);
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "JSSerial::write(): Argument #0 is undefined");
            return JS_FALSE;
        }

        asl::Ptr<asl::Block> myData(new Block());
        if (!convertFrom(cx, argv[0], myData)) {
            JS_ReportError(cx, "JSSerial::write(): Argument #1 must be a Block (Bytes to write)");
            return JS_FALSE;
        }

        JSSerial::getJSWrapper(cx,obj).openNative().write((const char*)myData->begin(), myData->size());
        JSSerial::getJSWrapper(cx,obj).closeNative();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
write(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Writes data to the serial device");
    DOC_PARAM("theData", "Bytes to write", DOC_TYPE_STRING);
    DOC_RESET;
    DOC_PARAM("theData", "Bytes to write (array of unsigned chars)", DOC_TYPE_ARRAY);
    DOC_END;
    try {
        if (argc != 1) {
            JS_ReportError(cx, "JSSerial::write(): Wrong number of arguments, "
                               "expected one (Bytes to write), got %d.", argc);
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "JSSerial::write(): Argument #0 is undefined");
            return JS_FALSE;
        }

        string myString;
        if (JSVAL_IS_STRING(argv[0])) {
            if (!convertFrom(cx, argv[0], myString)) {
                JS_ReportError(cx, "JSSerial::write(): Argument #1 must be an string or an array of unsigned char (Bytes to write)");
                return JS_FALSE;
            }
        } else {
            if (JSA_charArrayToString(cx, argv, myString) == JS_FALSE) {
                return JS_FALSE;
            }

            if (myString.empty()) {
                JS_ReportError(cx, "JSSerial::write(): Argument #1 must be an string or an array of unsigned char (Bytes to write)");
                return JS_FALSE;
            }
        }

        JSSerial::getJSWrapper(cx,obj).openNative().write(myString.c_str(), myString.size());
        JSSerial::getJSWrapper(cx,obj).closeNative();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
receivePacket(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Recieve one packet from the serial device.");
    DOC_RVAL("The oldest packet in the queue, or empty string if there are no packets in the queue", DOC_TYPE_STRING);
    DOC_END;
    try {
        if (argc != 0) {
            JS_ReportError(cx, "JSSerial::receivePacket(): Wrong number of arguments, expected none, got %d.", argc);
            return JS_FALSE;
        }

        uCCP & myUCCP = JSSerial::getObject(cx, obj).getUCCP();
        myUCCP.recive();

        string myPayload = "";
        if (myUCCP.pendingPackets() > 0) {
            myPayload = myUCCP.popPacket();
        }

        *rval = as_jsval(cx, myPayload);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
sendPacket(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sends a string over the serial device which is packeted with the specified packet format.");
    DOC_PARAM("theData", "String to send", DOC_TYPE_STRING);
    DOC_RESET;
    DOC_PARAM("theData", "Array of unsigned char to send", DOC_TYPE_ARRAY);
    DOC_END;
    try {
        if (argc != 1) {
            JS_ReportError(cx, "JSSerial::sendPacket(): Wrong number of arguments, "
                               "expected one (Bytes to send), got %d.", argc);
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "JSSerial::sendPacket(): Argument #0 is undefined");
            return JS_FALSE;
        }

        string myString;
        if (JSVAL_IS_STRING(argv[0])) {
            if (!convertFrom(cx, argv[0], myString)) {
                JS_ReportError(cx, "JSSerial::sendPacket(): Argument #1 must be an string or an array of unsigned char (Bytes to write)");
                return JS_FALSE;
            }
        } else {
            if (JSA_charArrayToString(cx, argv, myString) == JS_FALSE) {
                return JS_FALSE;
            }

            if (myString.empty()) {
                JS_ReportError(cx, "JSSerial::sendPacket(): Argument #1 must be an string or an array of unsigned char (Bytes to write)");
                return JS_FALSE;
            }
        }

        JSSerial::getObject(cx, obj).getUCCP().send(myString);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
printPacketStatistic(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Prints out statistics about send/received/lost packets");
    DOC_END;
    try {
        if (argc != 0) {
            JS_ReportError(cx, "JSSerial::printPacketStatistic(): Wrong number of arguments, expected none, got %d.", argc);
            return JS_FALSE;
        }

        JSSerial::getObject(cx, obj).getUCCP().printStatistic(cerr);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
printPacketFormat(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Prints the current packet format as ascii graphic");
    DOC_END;
    try {
        if (argc != 0) {
            JS_ReportError(cx, "JSSerial::printPacketFormat(): Wrong number of arguments, expected none, got %d.", argc);
            return JS_FALSE;
        }

        JSSerial::getObject(cx, obj).getUCCP().printPacketFormat(cerr);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSSerial::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"open",                 open,                    7},
        {"close",                close,                   0},
        {"read",                 read,                    1},
        {"readBlock",            readBlock,               1},
        {"flush",                flush,                   0},
        {"write",                write,                   1},
        {"writeBlock",           writeBlock,              1},
        {"receivePacket",        receivePacket,           0},
        {"setPacketFormat",      setPacketFormat,         4},
        {"sendPacket",           sendPacket,              1},
        {"printPacketStatistic", printPacketStatistic,    0},
        {"printPacketFormat",    printPacketFormat,       0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSSerial::Properties() {
    static JSPropertySpec myProperties[] = {
        {"isOpen", PROP_isOpen, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"status", PROP_status, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSSerial::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_isOpen:
            *vp = as_jsval(cx, getNative().isOpen());
            return JS_TRUE;
        case PROP_status:
            {
                JSClassTraits<NATIVE>::ScopedNativeRef myObjRef(cx, obj);
                *vp = as_jsval(cx, myObjRef.getNative().getStatusLine());
            }
            return JS_TRUE;
        default:
            JS_ReportError(cx,"JSSerial::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSSerial::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_isOpen:
            //jsval dummy;
            //return Method<NATIVE>::call(&NATIVE::isOpen, cx, obj, 1, vp, &dummy);
            return JS_FALSE;
        case PROP_status:
            {
                jsval dummy;
                return Method<NATIVE>::call(&NATIVE::setStatusLine, cx, obj, 1, vp, &dummy);
            }
        default:
            JS_ReportError(cx,"JSSerial::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSSerial::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a new serial device");
    DOC_PARAM("thePort", "Zero based com-port number or device name. Use port '999' to create a debug loopback device.", DOC_TYPE_INTEGER);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSSerial * myNewObject = 0;

    if (argc == 1) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx,"JSSerial::Constructor: bad argument #1 (undefined)");
            return JS_FALSE;
        }

        unsigned myPortNumber = 0;
        string myDeviceName;
        if (convertFrom(cx, argv[0], myPortNumber)) {
            if (myPortNumber == 999) {
                OWNERPTR myNewSerial = OWNERPTR(new DebugPort("MyDebug Port #999"));
                myNewObject = new JSSerial(myNewSerial, myNewSerial.get());
            } else {
                OWNERPTR myNewSerial = OWNERPTR(getSerialDevice(myPortNumber));
                myNewObject = new JSSerial(myNewSerial, myNewSerial.get());
            }
        } else if (convertFrom(cx, argv[0], myDeviceName)) {
            OWNERPTR myNewSerial = OWNERPTR(getSerialDeviceByName( myDeviceName ));
            myNewObject = new JSSerial(myNewSerial, myNewSerial.get());
        } else {
            JS_ReportError(cx, "JSSerial::Constructor: argument #1 must be an integer (port number)"
                                " or a string (device name).");
            return JS_FALSE;
        }

    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 1 (PortNumber) %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSSerial::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSSerial::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        {"NO_PARITY",         PROP_NO_PARITY,         SerialDevice::NO_PARITY},
        {"EVEN_PARITY",       PROP_EVEN_PARITY,       SerialDevice::EVEN_PARITY},
        {"ODD_PARITY",        PROP_ODD_PARITY,        SerialDevice::ODD_PARITY},
        {"NO_CHECKING",       PROP_NO_CHECKING,       uCCP::NO_CHECKING},
        {"CRC8_CHECKING",     PROP_CRC8_CHECKING,     uCCP::CRC8_CHECKING},
        {"CHECKSUM_CHECKING", PROP_CHECKSUM_CHECKING, uCCP::CHECKSUM_CHECKING},
        {"CTS",               PROP_STATUS_CTS,        SerialDevice::CTS},
        {"DSR",               PROP_STATUS_DSR,        SerialDevice::DSR},
        {"RI",                PROP_STATUS_RI,         SerialDevice::RI},
        {"CD",                PROP_STATUS_CD,         SerialDevice::CD},
        {"DTR",               PROP_STATUS_DTR,        SerialDevice::DTR},
        {"RTS",               PROP_STATUS_RTS,        SerialDevice::RTS},
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSSerial::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

static JSBool
getSerialDeviceNames(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the names of the serial ports on the system");
    DOC_END;
    try {
        std::vector<std::string> myPortNames;
        if (asl::getSerialDeviceNames(myPortNames)) {
             *rval = as_jsval(cx, myPortNames);
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

JSFunctionSpec *
JSSerial::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        // name                    native               nargs
        {"getSerialDeviceNames",   getSerialDeviceNames,  0},
        {0}};
    return myFunctions;
}

JSObject *
JSSerial::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties(),0,StaticFunctions());
    DOC_CREATE(JSSerial);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSSerial::NATIVE & theSerial) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSSerial::NATIVE >::Class()) {
                theSerial = JSClassTraits<JSSerial::NATIVE>::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSSerial::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSSerial::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSSerial::OWNERPTR theOwner, JSSerial::NATIVE * theSerial) {
    JSObject * myObject = JSSerial::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}

}
