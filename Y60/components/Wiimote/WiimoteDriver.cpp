//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "WiimoteDriver.h"

#include <y60/JSScriptablePlugin.h>
#include <y60/Documentation.h>

using namespace jslib;

namespace y60 {

JSBool
WiimoteDriver::SetRumble(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
    DOC_BEGIN("");
    DOC_END;
   
    asl::Ptr<Wiimote> myNative = getNativeAs<Wiimote>(cx, obj);

    int myId;
    convertFrom(cx, argv[0], myId );

    bool myFlag;
    convertFrom(cx, argv[1], myFlag );

    if (myNative) {
        myNative->setRumble(myId, myFlag);
    } else {
        assert(myNative);
    }
    return JS_TRUE;
}


JSBool
WiimoteDriver::RequestStatusReport(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
    DOC_BEGIN("");
    DOC_END;
   
    asl::Ptr<Wiimote> myNative = getNativeAs<Wiimote>(cx, obj);

    int myId;
    convertFrom(cx, argv[0], myId );

    if (myNative) {
        myNative->requestStatusReport(myId);
    } else {
        assert(myNative);
    }
    return JS_TRUE;
}

    
JSBool
WiimoteDriver::SetLEDs(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
    DOC_BEGIN("");
    DOC_END;
   
    asl::Ptr<Wiimote> myNative = getNativeAs<Wiimote>(cx, obj);

    int myId;
    convertFrom(cx, argv[0], myId );

    int myLED0;
    convertFrom(cx, argv[1], myLED0 );
    int myLED1;
    convertFrom(cx, argv[2], myLED1 );
    int myLED2;
    convertFrom(cx, argv[3], myLED2 );
    int myLED3;
    convertFrom(cx, argv[4], myLED3 );

    if (myNative) {
        myNative->setLEDs(myId, myLED0, myLED1, myLED2, myLED3);
    } else {
        assert(myNative);
    }
    return JS_TRUE;
}

} // end of namespace y60

extern "C"
EXPORT asl::PlugInBase* Wiimote_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::Wiimote(myDLHandle);
}
