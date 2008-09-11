//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSOscReceiver.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <y60/jsbase/JSNode.h>

using namespace std;
using namespace y60;

namespace jslib {

    template class JSWrapper<y60::OscReceiver, y60::OscReceiverPtr, jslib::StaticAccessProtocol>;

    static JSBool
    Stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
        DOC_BEGIN("Start listening for osc events in a seperate thread (fork)");
        DOC_END;
   
        return Method<JSOscReceiver::NATIVE>::call(&JSOscReceiver::NATIVE::stop,cx,obj,argc,argv,rval);
    }

    static JSBool
    Start(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
        DOC_BEGIN("Stop listening for osc events in a seperate thread (join).");
        DOC_END;
        AC_DEBUG << "JSOscReceiver Start";

        return Method<JSOscReceiver::NATIVE>::call(&JSOscReceiver::NATIVE::start,cx,obj,argc,argv,rval);
    }

    JSFunctionSpec * 
    JSOscReceiver::Functions() {
        static JSFunctionSpec myFunctions[] = {
            {"start", Start, 0},
            {"stop", Stop, 0},
            {0}
        };
        return myFunctions;
    }

    JSConstIntPropertySpec *
    JSOscReceiver::ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSPropertySpec *
    JSOscReceiver::Properties() {
        static JSPropertySpec myProperties[] = {
            {0}
        };
        return myProperties;
    }

    JSPropertySpec *
    JSOscReceiver::StaticProperties() {
        static JSPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSFunctionSpec *
    JSOscReceiver::StaticFunctions() {
        static JSFunctionSpec myFunctions[] = {{0}};
        return myFunctions;
    }
    // getproperty handling
    JSBool
    JSOscReceiver::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        switch (theID) {
        default:
            JS_ReportError(cx,"JSOscReceiver::getProperty: index %d out of range", theID);
            return JS_FALSE;
        }
    }

    // setproperty handling
    JSBool
    JSOscReceiver::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        return JS_FALSE;
    }



    JSBool
    JSOscReceiver::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Creates a OscReceiver with given port.");
        DOC_PARAM("thePort", "", DOC_TYPE_INTEGER);
        DOC_END;
        try {
            if (JSA_GetClass(cx,obj) != Class()) {
                JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
                return JS_FALSE;
            }

            int myPort;
            if (argc == 1) {
                if (!convertFrom(cx, argv[0], myPort)) {
                    JS_ReportError(cx, "JSOscReceiver::Constructor(): argument #1 must be the port as integer");
                    return JS_FALSE;
                }
            }

            OWNERPTR myNewNative;
            if (argc != 1) {
                // Construct empty OscReceiver that will be filled by copy Construct()
                AC_PRINT << "JSOscReceiver::Constructor: empty";
                myNewNative = OWNERPTR(0);
            } else {
                myNewNative = OscReceiverPtr(new OscReceiver(myPort));
            }

            JSOscReceiver * myNewObject = new JSOscReceiver(myNewNative, &(*myNewNative));
            if (myNewObject) {
                JS_SetPrivate(cx, obj, myNewObject);
                return JS_TRUE;
            } else  {
                JS_ReportError(cx,"JSOscReceiver::Constructor: bad parameters");
                return JS_FALSE;
            }
        } HANDLE_CPP_EXCEPTION;

    }

    JSObject *
    JSOscReceiver::initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
        DOC_MODULE_CREATE("Components", JSOscReceiver);
        return myClass;
    }

    jsval as_jsval(JSContext *cx, JSOscReceiver::OWNERPTR theOwner) {
        //AC_PRINT << "JSOscReceiver::as_jsval";
        JSObject * myReturnObject = JSOscReceiver::Construct(cx, theOwner, &(*theOwner));
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSOscReceiver::OWNERPTR theOwner, JSOscReceiver::NATIVE * theNative) {
        //AC_PRINT << "JSOscReceiver::as_jsval with Native";
        JSObject * myObject = JSOscReceiver::Construct(cx, theOwner, theNative);
        return OBJECT_TO_JSVAL(myObject);
    }


}
