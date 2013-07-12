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

#include "JSWebSocketClient.h"
#include "NetAsync.h"

#include <y60/jsbase/JSWrapper.impl>
#include <y60/jsbase/JSBlock.h>
#include <y60/jsbase/JSNode.h>

#include <iostream>

#include <netsrc/spidermonkey/jsfun.h>

using namespace std;
using namespace asl;
using namespace jslib;
using namespace y60;


#define DB(x) // x

// Lifecycle Debug
//#define LIFECYCLE_DEBUG

#ifdef LIFECYCLE_DEBUG
#define LCDB(x) x
size_t LC_COUNT = 0;
#else
#define LCDB(x) // x
#endif


static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns information on websocket client."); DOC_END;
    const async::websocket::Client & myClient = JSWebSocketClient::getJSWrapper(cx,obj).getNative();
    *rval = as_jsval(cx, myClient.debugIdentifier);
    return JS_TRUE;
}

static JSBool
send(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sends a string or a block to the server."); DOC_END;
    ensureParamCount(argc, 1);
    JSWebSocketClient::OWNERPTR nativePtr = JSClassTraits<JSWebSocketClient::NATIVE>::getNativeOwner(cx,obj);

    std::string stringData;
    asl::Ptr<asl::Block> binaryData;

    if (JSVAL_IS_STRING(argv[0]) && convertFrom(cx, argv[0], stringData)) {
        nativePtr->send(stringData);
    } else if (convertFrom(cx, argv[0], binaryData)) {
        nativePtr->send(*binaryData);
    } else {
        JS_ReportError(cx, "WebSocketClient::send: argument #1 is not a string or block");
        return JS_FALSE;
    }
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

JSWebSocketClient::JSWebSocketClient(OWNERPTR theOwner, NATIVE * theNative)
            : Base(theOwner, theNative)
{
    LCDB(AC_PRINT << "JSWebSocketClient CTOR, Count is now " << ++LC_COUNT);
    asl::Ptr<NetAsync> parentPlugin = dynamic_cast_Ptr<NetAsync>(Singleton<PlugInManager>::get().getPlugIn(NetAsync::PluginName));
    parentPlugin->getWSManager().addClient(theOwner);
};

JSWebSocketClient::~JSWebSocketClient() {
    asl::Ptr<NetAsync> parentPlugin = dynamic_cast_Ptr<NetAsync>(Singleton<PlugInManager>::get().getPlugIn(NetAsync::PluginName));
    parentPlugin->getWSManager().removeClient(getOwner());
    LCDB(AC_PRINT << "JSWebSocketClient DTOR, Count is now " << --LC_COUNT);
}

JSFunctionSpec *
JSWebSocketClient::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"send",                 send,                    1},
//        {"close",                close,                   1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSWebSocketClient::Properties() {
    static JSPropertySpec myProperties[] = {
        {"onopen", PROP_onopen, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"onclose", PROP_onclose, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"onmessage", PROP_onmessage, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"onerror", PROP_onerror, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSWebSocketClient::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSFunctionSpec *
JSWebSocketClient::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        {0}
    };
    return myFunctions;
}

JSConstIntPropertySpec *
JSWebSocketClient::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

// getproperty handling
JSBool
JSWebSocketClient::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_onopen:
                *vp = getNative().getJSOption("onopen");
                return JS_TRUE;
            case PROP_onclose:
                *vp = getNative().getJSOption("onclose");
                return JS_TRUE;
            case PROP_onmessage:
                *vp = getNative().getJSOption("onmessage");
                return JS_TRUE;
            case PROP_onerror:
                *vp = getNative().getJSOption("onerror");
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSRequestWrapper::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
    return JS_TRUE;
}

// setproperty handling
JSBool
JSWebSocketClient::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    switch (theID) {
        case PROP_onopen:
            {
                return myObj.getNative().setJSOption("onopen", vp); 
            }
        case PROP_onclose:
            {
                return myObj.getNative().setJSOption("onclose", vp); 
            }
        case PROP_onmessage:
            {
                return myObj.getNative().setJSOption("onmessage", vp); 
            }
        case PROP_onerror:
            {
                return myObj.getNative().setJSOption("onerror", vp); 
            }
    }
    return JS_TRUE;
}


JSBool
JSWebSocketClient::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a WebSocketClient.");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }
    ensureParamCount(argc, 1);

    JSObject* optsObject;
    string theURL;
    if (JSVAL_IS_STRING(argv[0])) {
        // construnctor was called with a string (uri) argument. We will transform this to a options-hash 
        // and then continue
        optsObject = JS_NewObject(cx, NULL, NULL, NULL);
        JS_SetProperty(cx, optsObject, "url", &argv[0]);
    } else if (JSVAL_IS_OBJECT(argv[0]) && !JSVAL_IS_NULL(argv[0])) {
        JS_ValueToObject(cx,argv[0],&optsObject);
    } else {
        JS_ReportError(cx, "WebSocketClient::Construct: argument #1 is not an object");
        return JS_FALSE;
    } 
    
    JSWebSocketClient * myNewObject = 0;
    boost::asio::io_service & io = dynamic_cast_Ptr<NetAsync>(Singleton<PlugInManager>::get().getPlugIn(NetAsync::PluginName))->io_service();
    OWNERPTR myWebSocketClient = OWNERPTR(new async::websocket::Client(cx, optsObject, io));
    myNewObject = new JSWebSocketClient(myWebSocketClient, myWebSocketClient.get());
    JS_SetPrivate(cx, obj, myNewObject);
    myWebSocketClient->setWrapper(obj);
    // now stick the opts object to the JSWebSocketClient wrapper so it's not GC'ed
    jsval optsValue = OBJECT_TO_JSVAL(optsObject);
    JS_SetProperty(cx, obj, "_opts", &optsValue);

    // initiate connection
    myWebSocketClient->connect();
    return JS_TRUE;
}


JSObject *
JSWebSocketClient::initClass(JSContext *cx, JSObject *theGlobalObject) {    
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    DOC_MODULE_CREATE("WebSocketClient", JSWebSocketClient);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSWebSocketClient::OWNERPTR & theNativePtr) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSWebSocketClient::NATIVE>::Class()) {
                theNativePtr = JSClassTraits<JSWebSocketClient::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSWebSocketClient::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSWebSocketClient::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}


jsval as_jsval(JSContext *cx, JSWebSocketClient::OWNERPTR theOwner, JSWebSocketClient::NATIVE * theSerial) {
    JSObject * myObject = JSWebSocketClient::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}
