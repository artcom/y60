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

#include "JSSynergyServer.h"

using namespace jslib;

static JSBool
onMouseMotion(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
    DOC_BEGIN("Sends mouse move data to synergy clients.");
    DOC_END;

    return Method<JSSynergyServer::NATIVE>::call( &JSSynergyServer::NATIVE::onMouseMotion,
                                                  cx, obj, argc, argv, rval);
}

static JSBool
onRelMouseMotion(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
    DOC_BEGIN("Sends relative mouse move data to synergy clients.");
    DOC_END;

    return Method<JSSynergyServer::NATIVE>
           ::call( &JSSynergyServer::NATIVE::onRelMouseMotion,
                   cx, obj, argc, argv, rval);
}

static JSBool
onMouseButton(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
    DOC_BEGIN("Sends mouse button data to synergy clients.");
    DOC_END;

    return Method<JSSynergyServer::NATIVE>::call( &JSSynergyServer::NATIVE::onMouseButton,
                                                  cx, obj, argc, argv, rval);
}

static JSBool
onMouseWheel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
    DOC_BEGIN("Sends mouse wheel data to synergy clients.");
    DOC_END;

    return Method<JSSynergyServer::NATIVE>::call( &JSSynergyServer::NATIVE::onMouseWheel,
                                                  cx, obj, argc, argv, rval);
}

static JSBool
getScreenSize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
    DOC_BEGIN("Get the screen size from the synergy client.");
    DOC_END;

    return Method<JSSynergyServer::NATIVE>::call( &JSSynergyServer::NATIVE::getScreenSize,
                                                  cx, obj, argc, argv, rval);
}

static JSBool
isConnected(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
    DOC_BEGIN("Returns true if the handshake with the client software is complete.");
    DOC_END;

    return Method<JSSynergyServer::NATIVE>::call( &JSSynergyServer::NATIVE::isConnected,
                                                  cx, obj, argc, argv, rval);
}


JSSynergyServer::~JSSynergyServer() {
}

JSFunctionSpec *
JSSynergyServer::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name          native         nargs
        {"onMouseMotion",        onMouseMotion,         2},
        {"onRelMouseMotion",     onRelMouseMotion,      2},
        {"onMouseButton",        onMouseButton,         2},
        {"onMouseWheel",         onMouseWheel,          2},
        {"getScreenSize",        getScreenSize,         2},
        {"isConnected",          isConnected,           2},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSSynergyServer::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSSynergyServer::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSFunctionSpec *
JSSynergyServer::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        // name             native           nargs
        {0}
    };
    return myFunctions;
}

// getproperty handling
JSBool
JSSynergyServer::getPropertySwitch( unsigned long theID, JSContext *cx, JSObject *obj,
                                    jsval id, jsval *vp)
{
    //switch (theID) {
    //default:
        JS_ReportError(cx,"JSSynergyServer::getProperty: index %d out of range", theID);
        return JS_FALSE;
    //}
}

// setproperty handling
JSBool
JSSynergyServer::setPropertySwitch( unsigned long theID, JSContext *cx, JSObject *obj,
                                    jsval id, jsval *vp)
{
    //switch (theID) {
    //default:
        JS_ReportError( cx,"JSSynergyServer::setPropertySwitch: index %d out of range",
                        theID);
        return JS_FALSE;
    //}
}

JSBool
JSSynergyServer::Constructor( JSContext *cx, JSObject *obj, uintN argc, jsval *argv,
                              jsval *rval)
{
    DOC_BEGIN("Creates a SynergyServer Device.");
    DOC_PARAM_OPT("theHostName", "The host name of the server.", DOC_TYPE_STRING,
                  "localhost");
    DOC_PARAM_OPT("thePort", "The port of the server.", DOC_TYPE_STRING, "24800");
    DOC_END;

    try{

        ensureParamCount(argc, 2);

        if (JSA_GetClass(cx,obj) != Class()) {
            JS_ReportError( cx, "Constructor for %s bad object; did you forget a 'new'?",
                            ClassName());
            return JS_FALSE;
        }

        JSSynergyServer * myNewObject = 0;

        std::string myHostName;
        if (!convertFrom(cx, argv[0], myHostName)) {
            JS_ReportError( cx,
                            "JSSynergyServer::Constructor: argument #1 must be a "
                            "string)" );
            return JS_FALSE;
        }
        asl::Unsigned32 myHostAddress = inet::getHostAddress( myHostName.c_str() );

        unsigned myPort;
        if (!convertFrom(cx, argv[1], myPort)) {
            JS_ReportError( cx,
                            "JSSynergyServer::Constructor: argument #2 must be a "
                            "Number)" );
            return JS_FALSE;
        }

        OWNERPTR mySynergyServer = OWNERPTR(new SynergyServer( myHostAddress,
                                                               static_cast<asl::Unsigned16>(myPort) ));
        myNewObject = new JSSynergyServer(mySynergyServer, mySynergyServer.get());

        if (myNewObject) {
            JS_SetPrivate(cx, obj, myNewObject);
            return JS_TRUE;
        }

        JS_ReportError(cx,"JSSynergyServer::Constructor: bad parameters");

        return JS_FALSE;

    } HANDLE_CPP_EXCEPTION;
}

JSConstIntPropertySpec *
JSSynergyServer::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

JSObject *
JSSynergyServer::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass( cx, theGlobalObject, ClassName(), Constructor,
                                         Properties(), Functions(), ConstIntProperties(),
                                         0, StaticFunctions());
    DOC_MODULE_CREATE("SynergyServer", JSSynergyServer);
    return myClass;
}

jsval as_jsval(JSContext *cx, JSSynergyServer::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSSynergyServer::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}


jsval as_jsval( JSContext *cx, JSSynergyServer::OWNERPTR theOwner,
                JSSynergyServer::NATIVE * theSerial )
{
    JSObject * myObject = JSSynergyServer::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}


extern "C"
EXPORT asl::PlugInBase * SynergyServer_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::JSSynergyServerPlugIn(myDLHandle);
}
