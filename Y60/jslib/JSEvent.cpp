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
//   $RCSfile: JSEvent.cpp,v $
//   $Author: christian $
//   $Revision: 1.4 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSEvent.h"
#include "JSNode.h"
#include "JScppUtils.h"

#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = std::string("Event<")+JSEvent::getJSWrapper(cx,obj).getNative().type()+">";
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}
#if 0
static JSBool
open(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        unsigned myBaudRate    = DEFAULT_BAUD_RATE;
        unsigned myBits        = DEFAULT_BITS;
        unsigned myParity      = DEFAULT_PARITY;
        unsigned myStopBits    = DEFAULT_STOP_BITS;
        bool     myHwHandshake = DEFAULT_HW_HANDSHAKE;

        for (unsigned i = 0; i < argc; ++i) {
            if (JSVAL_IS_VOID(argv[i])) {
                JS_ReportError(cx, "JSEvent::open(): Argument #%d is undefined", i + 1);
                return JS_FALSE;
            }
        }

        if (argc > 0) {
            if (!convertFrom(cx, argv[0], myBaudRate)) {
                JS_ReportError(cx, "JSEvent::open(): argument #1 must be an integer (Baud Rate)");
                return JS_FALSE;
            }
        }
        if (argc > 1) {
            if (!convertFrom(cx, argv[1], myBits)) {
                JS_ReportError(cx, "JSEvent::open(): argument #2 must be an integer (Data Bits)");
                return JS_FALSE;
            }
        }
        if (argc > 2) {
            if (!convertFrom(cx, argv[2], myParity)) {
                JS_ReportError(cx, "JSEvent::open(): argument #3 must be an integer (Parity Mode)");
                return JS_FALSE;
            }
            if (myParity > 2) {
                JS_ReportError(cx, "JSEvent::open(): Illegal parity mode %d, "
                                   "must be between NO_PARITY, EVEN_PARITY or ODD_PARITY", myParity);
                return JS_FALSE;
            }
        }
        if (argc > 3) {
            if (!convertFrom(cx, argv[3], myStopBits)) {
                JS_ReportError(cx, "JSEvent::open(): argument #4 must be an integer (Stop Bits)");
                return JS_FALSE;
            }
        }
        if (argc > 4) {
            if (!convertFrom(cx, argv[4], myHwHandshake)) {
                JS_ReportError(cx, "JSEvent::open(): argument #5 must be a boolean (Hardware Handshake)");
                return JS_FALSE;
            }
        }
        if (argc > 5) {
            JS_ReportError(cx, "JSEvent::open(): Wrong number of arguments, between 0 and 5 expected.");
            return JS_FALSE;
        }

        JSEvent::getJSWrapper(cx,obj).openNative().open(myBaudRate, myBits,
            EventDevice::ParityMode(myParity), myStopBits, myHwHandshake);
        JSEvent::getJSWrapper(cx,obj).closeNative();

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<JSEvent::NATIVE>::call(&JSEvent::NATIVE::close,cx,obj,argc,argv,rval);
}

static JSBool
peek(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<JSEvent::NATIVE>::call(&JSEvent::NATIVE::peek,cx,obj,argc,argv,rval);
}

static JSBool
setPacketFormat(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        unsigned char myStartByte;
        unsigned char myEndByte;
        unsigned      mySize;
        unsigned      myErrorChecking;

        for (unsigned i = 0; i < argc; ++i) {
            if (JSVAL_IS_VOID(argv[i])) {
                JS_ReportError(cx, "JSEvent::setPacketFormat(): Argument #%d is undefined", i + 1);
                return JS_FALSE;
            }
        }

        if (argc != 4) {
            JS_ReportError(cx, "JSEvent::setPacketFormat(): expected 4 arguments, got %d. "
                               "Usage: (StartByte, EndByte, Size, Errorchecking)", argc);
            return JS_FALSE;
        }

        uCCP & myUCCP = JSEvent::getObject(cx, obj).getUCCP();

        if (!convertFrom(cx, argv[0], myStartByte)) {
            JS_ReportError(cx, "JSEvent::setPacketFormat(): argument #1 must be an integer (Start Byte)");
            return JS_FALSE;
        }
        if (!convertFrom(cx, argv[1], myEndByte)) {
            JS_ReportError(cx, "JSEvent::setPacketFormat(): argument #2 must be an integer (End Byte)");
            return JS_FALSE;
        }

        myUCCP.setFrame(myStartByte, myEndByte);

        if (!convertFrom(cx, argv[2], mySize)) {
            JS_ReportError(cx, "JSEvent::setPacketFormat(): argument #3 must be an integer (payload size)");
            return JS_FALSE;
        }

        if (mySize == 0) {
            myUCCP.setVariablePayloadSize();
        } else {
            myUCCP.setFixedPayloadSize(mySize);
        }

        if (!convertFrom(cx, argv[3], myErrorChecking)) {
            JS_ReportError(cx, "JSEvent::setPacketFormat(): argument #4 must be an integer (errorchecking)");
            return JS_FALSE;
        }

        if (myErrorChecking > 2) {
            JS_ReportError(cx, "JSEvent::setPacketFormat(): Invalid error checking format");
            return JS_FALSE;
        }

        myUCCP.setErrorChecking(uCCP::ErrorChecking(myErrorChecking));

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
read(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        if (argc > 1) {
            JS_ReportError(cx, "JSEvent::read(): Wrong number of arguments, expected one or none, got %d.", argc);
            return JS_FALSE;
        }

        unsigned myReadBytes = READ_BUFFER_SIZE;
        if (argc == 1) {
             if (!convertFrom(cx, argv[0], myReadBytes)) {
                JS_ReportError(cx, "JSEvent::write(): Argument #1 must be a unsigned (bytes to read)");
                return JS_FALSE;
            }
        }


        string myResult;
        char myBuffer[READ_BUFFER_SIZE];

        do {
            JSEvent::getJSWrapper(cx,obj).openNative().read(myBuffer, myReadBytes);
            JSEvent::getJSWrapper(cx,obj).closeNative();
            myResult.append(myBuffer, myReadBytes);
        } while (myReadBytes == READ_BUFFER_SIZE);

        JSString * myString = JS_NewStringCopyN(cx, myResult.c_str(), myResult.size());
        *rval = STRING_TO_JSVAL(myString);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
write(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        if (argc != 1) {
            JS_ReportError(cx, "JSEvent::write(): Wrong number of arguments, "
                               "expected one (Bytes to write), got %d.", argc);
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "JSEvent::write(): Argument #0 is undefined");
            return JS_FALSE;
        }

        string myString;
        if (JSVAL_IS_STRING(argv[0])) {
            if (!convertFrom(cx, argv[0], myString)) {
                JS_ReportError(cx, "JSEvent::write(): Argument #1 must be an string or an array of unsigned char (Bytes to write)");
                return JS_FALSE;
            }
        } else {
            if (JSA_charArrayToString(cx, argv, myString) == JS_FALSE) {
                return JS_FALSE;
            }

            if (myString.empty()) {
                JS_ReportError(cx, "JSEvent::write(): Argument #1 must be an string or an array of unsigned char (Bytes to write)");
                return JS_FALSE;
            }
        }

        JSEvent::getJSWrapper(cx,obj).openNative().write(myString.c_str(), myString.size());
        JSEvent::getJSWrapper(cx,obj).closeNative();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
receivePacket(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        if (argc != 0) {
            JS_ReportError(cx, "JSEvent::receivePacket(): Wrong number of arguments, expected none, got %d.", argc);
            return JS_FALSE;
        }

        uCCP & myUCCP = JSEvent::getObject(cx, obj).getUCCP();
        myUCCP.recive();

        string myPayload = "";
        if (myUCCP.pendingPackets() > 0) {
            myPayload = myUCCP.popPacket();
        }

        JSString * myString = JS_NewStringCopyN(cx, myPayload.c_str(), myPayload.size());
        *rval = STRING_TO_JSVAL(myString);

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
sendPacket(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        if (argc != 1) {
            JS_ReportError(cx, "JSEvent::sendPacket(): Wrong number of arguments, "
                               "expected one (Bytes to send), got %d.", argc);
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "JSEvent::sendPacket(): Argument #0 is undefined");
            return JS_FALSE;
        }

        string myString;
        if (JSVAL_IS_STRING(argv[0])) {
            if (!convertFrom(cx, argv[0], myString)) {
                JS_ReportError(cx, "JSEvent::sendPacket(): Argument #1 must be an string or an array of unsigned char (Bytes to write)");
                return JS_FALSE;
            }
        } else {
            if (JSA_charArrayToString(cx, argv, myString) == JS_FALSE) {
                return JS_FALSE;
            }

            if (myString.empty()) {
                JS_ReportError(cx, "JSEvent::sendPacket(): Argument #1 must be an string or an array of unsigned char (Bytes to write)");
                return JS_FALSE;
            }
        }

        JSEvent::getObject(cx, obj).getUCCP().send(myString);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
setNoisy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        if (argc > 1) {
            JS_ReportError(cx, "JSEvent::setNoisy(): Wrong number of arguments, "
                               "expected one (noisy flag), got %d.", argc);
            return JS_FALSE;
        }

        bool myNoisyFlag;
        if (argc == 0) {
            myNoisyFlag = true;
        }

        if (argc == 1) {
            if (JSVAL_IS_VOID(argv[0])) {
                JS_ReportError(cx, "JSEvent::setNoisy(): Argument #0 is undefined");
                return JS_FALSE;
            }

            if (!convertFrom(cx, argv[0], myNoisyFlag)) {
                JS_ReportError(cx, "JSEvent::setNoisy(): Argument #1 must be boolean (noisy flag)");
                return JS_FALSE;
            }
        }

        JSEvent::getJSWrapper(cx,obj).openNative().setNoisy(myNoisyFlag);
        JSEvent::getJSWrapper(cx,obj).closeNative();
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
printPacketStatistic(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        if (argc != 0) {
            JS_ReportError(cx, "JSEvent::printPacketStatistic(): Wrong number of arguments, expected none, got %d.", argc);
            return JS_FALSE;
        }

        JSEvent::getObject(cx, obj).getUCCP().printStatistic(cerr);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
printPacketFormat(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    try {
        if (argc != 0) {
            JS_ReportError(cx, "JSEvent::printPacketFormat(): Wrong number of arguments, expected none, got %d.", argc);
            return JS_FALSE;
        }

        JSEvent::getObject(cx, obj).getUCCP().printPacketFormat(cerr);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}
#endif

static JSBool
initEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<dom::Event>::call(&dom::Event::initEvent, cx, obj, argc, argv, rval);
}
static JSBool
stopPropagation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<dom::Event>::call(&dom::Event::initEvent, cx, obj, argc, argv, rval);
}
static JSBool
preventDefault(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    return Method<dom::Event>::call(&dom::Event::initEvent, cx, obj, argc, argv, rval);
}

JSFunctionSpec *
JSEvent::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"initEvent",            initEvent,               3},
        {"stopPropagation",      stopPropagation,         0},
        {"preventDefault",       preventDefault,          0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSEvent::Properties() {
    static JSPropertySpec myProperties[] = {
        {"type", PROP_type, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"target", PROP_target, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"currentTarget", PROP_currentTarget, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"eventPhase", PROP_eventPhase, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"bubbles", PROP_bubbles, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"cancelable", PROP_cancelable, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"timeStamp", PROP_timeStamp, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"isDefaultPrevented", PROP_isDefaultPrevented, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSEvent::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        "CAPTURING_PHASE", PROP_CAPTURING_PHASE,  dom::Event::CAPTURING_PHASE,
        "AT_TARGET",       PROP_AT_TARGET,        dom::Event::AT_TARGET,
        "BUBBLING_PHASE",  PROP_BUBBLING_PHASE,   dom::Event::BUBBLING_PHASE,
        {0}
    };
    return myProperties;
};

JSPropertySpec *
JSEvent::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSEvent::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}
 
// getproperty handling
JSBool
JSEvent::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_target:
                *vp = as_jsval(cx, getNative().target());
                return JS_TRUE;
            case PROP_currentTarget:
                *vp = as_jsval(cx, getNative().currentTarget());
                return JS_TRUE;
            case PROP_eventPhase:
                *vp = as_jsval(cx, getNative().eventPhase());
                return JS_TRUE;
            case PROP_bubbles:
                *vp = as_jsval(cx, getNative().bubbles());
                return JS_TRUE;
            case PROP_cancelable:
                *vp = as_jsval(cx, getNative().cancelable());
                return JS_TRUE;
            case PROP_timeStamp:
                *vp = as_jsval(cx, getNative().timeStamp());
                return JS_TRUE;
            case PROP_isDefaultPrevented:
                *vp = as_jsval(cx, getNative().isDefaultPrevented());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSEvent::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSEvent::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    //switch (theID) {
        //case PROP_isOpen:
            //jsval dummy;
            //return Method<NATIVE>::call(&NATIVE::isOpen, cx, obj, 1, vp, &dummy);
            return JS_FALSE;
      //  default:
            JS_ReportError(cx,"JSEvent::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    //}
}

/* new Event(string theType,
             object thePayload = 0,
             bool canBubbleArg = true,
             bool cancelableArg = true,
             double theTimeStamp = DOMTimeStamp() )
*/
JSBool
JSEvent::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSEvent * myNewObject = 0;

    if (argc == 1) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx,"JSEvent::Constructor: bad argument #1 (undefined)");
            return JS_FALSE;
        }

        std::string myType = 0;
        if (!convertFrom(cx, argv[0], myType)) {
            JS_ReportError(cx, "JSEvent::Constructor: argument #1 must be an string (type)");
            return JS_FALSE;
        }
        asl::Ptr<JSObject*> myObject(new JSObject*(0));
        if (argc > 1) {
            *myObject = JSVAL_TO_OBJECT(argv[1]);
        }
        bool canBubbleArg = true;
        if (argc > 2) {
            if (!convertFrom(cx, argv[1], canBubbleArg)) {
                JS_ReportError(cx, "JSEvent::Constructor: argument #3 must be a bool (canBubbleArg)");
                return JS_FALSE;
            }
        }
        bool cancelableArg = true;
        if (argc > 3) {
            if (!convertFrom(cx, argv[3], cancelableArg)) {
                JS_ReportError(cx, "JSEvent::Constructor: argument #4 must be a bool (cancelableArg)");
                return JS_FALSE;
            }
        }
        dom::DOMTimeStamp myTimeStamp;
        double myTime;
        if (argc > 4) {
            if (!convertFrom(cx, argv[4], myTime)) {
                myTimeStamp = myTime;
                JS_ReportError(cx, "JSEvent::Constructor: argument #5 must be a double (theTimeStamp)");
                return JS_FALSE;
            }
        }
        if (argc > 5) {
            JS_ReportError(cx, "JSEvent::Constructor: excess arguments ignored, max arguments = 5");
        }

        OWNERPTR myNewEvent = OWNERPTR(
            new GenericJSEvent(myType,
                     canBubbleArg,
                     cancelableArg,
                     myTimeStamp,
                     myObject)
                     );
        myNewObject = new JSEvent(myNewEvent, &(*myNewEvent));
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 1-5 (theType [, thePayload, canBubbleArg, cancelableArg, theTimeStamp]) %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSEvent::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSEvent::initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor,
                Properties(), Functions(), ConstIntProperties());
        createClassDocumentation(ClassName(), Properties(), Functions(),
                ConstIntProperties(), 0, 0, 0);
        return myClass;
}

//bool convertFrom(JSContext *cx, jsval theValue, JSEvent::NATIVE & theEvent) {
bool convertFrom(JSContext *cx, jsval theValue, dom::EventPtr & theEvent) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSEvent::NATIVE >::Class()) {
                theEvent = JSClassTraits<JSEvent::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}


//jsval as_jsval(JSContext *cx, JSEvent::OWNERPTR theOwner) {
jsval as_jsval(JSContext *cx, dom::EventPtr theOwner) {
    JSObject * myReturnObject = JSEvent::Construct(cx, theOwner, 0);
    return OBJECT_TO_JSVAL(myReturnObject);
}

// jsval as_jsval(JSContext *cx, JSEvent::OWNERPTR theOwner, JSEvent::NATIVE * theEvent) {
jsval as_jsval(JSContext *cx, dom::EventPtr theOwner, JSEvent::NATIVE * theEvent) {
    JSObject * myObject = JSEvent::Construct(cx, theOwner, theEvent);
    return OBJECT_TO_JSVAL(myObject);
}

jsval as_jsval(JSContext *cx, dom::EventTargetPtr theOwner) {
    dom::NodePtr myNode = dynamic_cast_Ptr<dom::Node>(theOwner);
    if (myNode) {
        return as_jsval(cx, myNode);
    }
    AC_ERROR << "as_jsval(SContext *, dom::EventTargetPtr): not a Node" << endl;
    return JSVAL_VOID;
}

}
